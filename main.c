//Julio Villazón | A01370190 -> Trabajo hecho con Jesus Gonzalez
//Jesus Gonzalez | A01422050 -> Trabajo hecho con Julio Villazón

/*
Se utilizo el metodo de asignacion de PID manual a cada cliente para simplificar la inicializacion.
Se debe inicializar tanto el cliente como el servidor con la ip del localhost como argumente
El primer cliente debe ser inicializado con la opcion de -m (master)
Al indicarle a cada cliente el PID del vecino, el cliente inicializado como master debe recivir el PID del vecino como ultimo.
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define TCP_PORT 8000

int nClient = 0;
char tipoSenal;
int id;

void gestor(int);

int main(int argc, const char * argv[]) 
{    

    struct sigaction senal;

    senal.sa_handler = gestor;

    sigaction(SIGTSTP, &senal, 0);
    sigaction(SIGINT, &senal, 0);

    struct sockaddr_in direccion;
    char buffer;
    int continuar = 1;

    int server; 
    int client; 

    size_t leidos, escritos;
    pid_t pid;
    
    // Crear el socket
    server = socket(PF_INET, SOCK_STREAM, 0);

    // Enlace con el socket
    inet_aton(argv[1], &direccion.sin_addr);
    direccion.sin_port = htons(TCP_PORT);
    direccion.sin_family = AF_INET;
    
    bind(server, (struct sockaddr *) &direccion, sizeof(direccion));

    // Escuhar
    listen(server, 10);

    escritos = sizeof(direccion);

    // Aceptar conexiones
    while (continuar) 
    {
        client = accept(server, (struct sockaddr *) &direccion, &escritos);

        printf("Aceptando conexiones en %s:%d \n",
               inet_ntoa(direccion.sin_addr),
               ntohs(direccion.sin_port));

        nClient += 1;

        pid = fork();

        if (pid == 0) continuar = 0;
        
    }

    if (pid == 0) {

        close(server);

        if (client >= 0) {
            id = client;

            while ((leidos = read(client, &buffer, sizeof(buffer)))) {
                if (leidos == 1) 
                {
                    printf("semaforo numero: %d, cambio a : ", nClient);

                    switch (buffer) {
                        case 'R':
                            printf("Rojo! \n");
                            break;
                        case 'V':
                            printf("Verde! \n");
                            break;
                        case 'I':
                            printf("Intermitente! \n");
                            break;
                    }
                }
                buffer = '\0';
            }
        }

        close(client);
    } else if (pid > 0) 
    {
        while (wait(NULL) != -1);

        // Cerrar sockets
        close(server);
    }
    return 0;
}

void gestor(int signal) {
    if (signal == SIGTSTP)
    {
        tipoSenal = 'R';

    } else if (signal == SIGINT) 
    {
        tipoSenal = 'I';
    }
    write(id, &tipoSenal, sizeof(char));
}