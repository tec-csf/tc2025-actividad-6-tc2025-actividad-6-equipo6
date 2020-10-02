//Julio Villazón | A01370190 -> Trabajo hecho con Jesus Gonzalez
//Jesus Gonzalez | A01422050 -> Trabajo hecho con Julio Villazón

/*
Se utilizo el metodo de asignacion de PID manual a cada cliente para simplificar la inicializacion.
Se debe inicializar tanto el cliente como el servidor con la ip del localhost como argumente
El primer cliente debe ser inicializado con la opcion de -m (master)
Al indicarle a cada cliente el PID del vecino, el cliente inicializado como master debe recivir el PID del vecino como ultimo.
*/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define TCP_PORT 8000



int id;

int neighborPID;

char currentStatus = 'R';
char previousStatus;

void gestorAlarma(int);
void gestor(int);

int main(int argc, char * argv[]) 
{    
    struct sigaction senal;

    senal.sa_handler = gestor;

    sigaction(SIGUSR1, &senal, 0);

    struct sockaddr_in direccion;
    char buffer;
    char tipoSenal = '\0';
    char *valor;

    int opc;
    int flag = 0;
    int master;
    int client;

    ssize_t leidos, escritos;

    while ((opc = getopt(argc, argv, "m")) != -1) {
        switch (opc) {
            case 'm':
                master = 1;
                break;
            default:
                abort();
        }
    }

    // Crear el socket
    client = socket(PF_INET, SOCK_STREAM, 0);
    id = client;

    // Establecer conexión
    if (master == 1) 
    {
        inet_aton(argv[2], &direccion.sin_addr);
    } else 
    {
        inet_aton(argv[1], &direccion.sin_addr);
    }

    direccion.sin_port = htons(TCP_PORT);
    direccion.sin_family = AF_INET;

    escritos = connect(client, (struct sockaddr *) &direccion, sizeof(direccion));

    if (escritos == 0)  {
        printf("Conectado a %s:%d \n",
               inet_ntoa(direccion.sin_addr),
               ntohs(direccion.sin_port));

        printf("PID del cliente -> %d\n", getpid());
        printf("Ingresar el PID del cliente vecino: ");
        scanf("%d", &neighborPID);
        printf("Enlazado con el cliente -> PID %d\n", neighborPID);
        
        if (master == 1) 
        {
            printf("Color: %c\n", currentStatus);
            kill(neighborPID, SIGUSR1);
            master = 0;
        }
        // Escribir datos en el socket
        while ((leidos = read(client, &buffer, sizeof(buffer)))) {
            if (leidos == 1) { 
                if (tipoSenal != buffer) {
                    tipoSenal = buffer;
                    if (flag == 0) {
                        previousStatus = currentStatus;
                        flag = 1;
                    }
                    currentStatus = tipoSenal;
                    printf("Color: %c\n", currentStatus);
                    write(id, &currentStatus, sizeof(char));
                    alarm(0);
                } else {
                    tipoSenal = '\0';
                    currentStatus = previousStatus;
                    flag = 0;
                    if (currentStatus == 'V') {
                        printf("\nClientes reestablecidos\n\n");
                        raise(SIGUSR1);
                    }
                }
            }
        }
    }

    // Cerrar sockets
    close(client);
    
    return 0;
}

void gestorAlarma(int signal) {
    kill(neighborPID, SIGUSR1);
    currentStatus = 'R';

    printf("Color: %c\n", currentStatus);
    write(id, &currentStatus, sizeof(char));
}

void gestor(int signal) 
{
    struct sigaction senal;

    senal.sa_handler = gestorAlarma;       

    currentStatus = 'V';
    printf("Color: %c\n", currentStatus);

    write(id, &currentStatus, sizeof(char));
    alarm(8);
    sigaction(SIGALRM, &senal, 0);
}