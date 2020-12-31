#define _POSIX_SOURCE

#include <pthread.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/shm.h>
#include <features.h>

#include "../connectionUtils/socket.h"
#include "../fileParser.h"

//strings
#define START  "Starting Wendy...\n"
#define ERROR_FORK "Error al fer fork! \n"
#define NEW_CONNECTION "New Connection: "
#define SEM_CREATE_ERROR "Error en crear el semàfor \n"
#define ERR_OUT -1
#define DISCONNECTION_WENDY "Disconnecting Wendy...\n"

//Variables globals
int generalSocketFD;
int socketTemp;
int *forkIDsClients;
int forkCounter = 0;
pid_t parent_pid;

configJack config;

void signalhandler(int sigint){

    switch (sigint) {
        case SIGINT:
            free(forkIDsClients);

            free (config.nom);
            free (config.ipJack);

            write(1, DISCONNECTION_WENDY, sizeof(DISCONNECTION_WENDY));
            exit(0);
            break;
        default:
            break;
    }

    //Reassignem el nostre handler al signal
    signal(sigint, signalhandler);
}


int main(int argc, char *argv[]) {
    int bytes;
    char buff[100];

    //Fem una ràpida comprovació d'arguments
    if(argc < 2){
        char buff[100];
        int bytes = sprintf(buff, ARGUMENT_ERROR);
        write(1, buff, bytes);
        exit(ERROR_RETURN);
    }

    parent_pid = getpid();

    write(1, START, strlen(START));


    //Primer llegim la configuració
    config.nom = NULL;
    config.ipJack = NULL;
    config.portJack = 0;
    llegirConfig(argv[1], "Wendy", NULL, &config);

    //Iniciem el servidor
    generalSocketFD = iniciarServidor(config.ipJack, config.portJack);

    forkIDsClients = (int *) malloc(sizeof(int));

    //Reassingem interrupcions
    signal(SIGINT, signalhandler);


    //Esperem a rebre connexions
    while(1){
        write(1, "$Wendy:\n", sizeof("$Wendy:\n"));
        write(1, "Waiting...\n", sizeof("Waiting...\n"));

        socketTemp = acceptarConnexio(generalSocketFD);
        printf("-- %d\n", socketTemp);
        if (socketTemp < 0){
            bytes = sprintf(buff, ACCEPT_ERROR);
            write(1, buff, bytes);
            return -1;
        }

        forkIDsClients = (int *) realloc(forkIDsClients,(forkCounter+1)*sizeof(int));


        //Fork per tractar el socket
        forkIDsClients[forkCounter] = fork();
        switch (forkIDsClients[forkCounter]) {
            case -1:
                //Display error
                bytes = sprintf(buff, ERROR_FORK);
                write(1, buff, bytes);
                break;
            case 0:
                ;
                //FILL

                char *nomclient;
                nomclient = protocolconnexioServidor(socketTemp);
                if (strcmp(nomclient, "ERROR") == 0) return -1;
                write(1, NEW_CONNECTION, strlen(NEW_CONNECTION));
                write(1, nomclient, strlen(nomclient));
                write(1, "\n", sizeof("\n"));
                free(nomclient);
                gestionarClientWendy(socketTemp);
                return 0;
                break;

            default:
                //Pare
                forkCounter++;
                break;
        }
    }

    return 0;
}
