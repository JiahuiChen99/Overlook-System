#define _POSIX_SOURCE
#include <pthread.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

#include "../fileParser.h"
#include "../connectionUtils/socket.h"

//strings
#define START  "Starting Jack...\n"
#define ERROR_FORK "Error al fer fork! \n"
#define NEW_CONNECTION "New Connection: "

//Variables globals
int *socketsClients;
int *forkIDsClients;

void signalhandler(int sigint){

    switch (sigint) {

        case SIGALRM:


            break;
        case SIGINT:
            for(int i = 0; i < (int)((int)sizeof(socketsClients)/(int)sizeof(int)); i++){
                //Petem els fills
                kill(forkIDsClients[i], SIGKILL);
            }

            //Tanquem el servidor
            exit(0);
            break;
        default:
            break;
    }

    //Reassignem el nostre handler al signal
    signal(sigint, signalhandler);
}

int main(int argc, char *argv[]) {
    configJack config;
    int generalSocketFD, bytes;
    char buff[100];

    //Fem una ràpida comprovació d'arguments
    if(argc < 2){
        char buff[100];
        int bytes = sprintf(buff, ARGUMENT_ERROR);
        write(1, buff, bytes);
        exit(ERROR_RETURN);
    }

    write(1, START, strlen(START));


    //Primer llegim la configuració
    config.nom = NULL;
    config.ipJack = NULL;
    config.portJack = 0;
    llegirConfig(argv[1], "Jack", NULL, &config);

    //Iniciem el servidor
    generalSocketFD = iniciarServidor(config.ipJack, config.portJack);


    //Reassingem interrupcions
    signal(SIGINT, signalhandler);

    //Esperem a rebre
    socketsClients = (int *) malloc(sizeof(int));
    forkIDsClients = (int *) malloc(sizeof(int));
    int socketCounter = 0;
    int forkCounter = 0;

    while(1){
        //Fer un accept
        struct sockaddr_in cli_addr;
        socklen_t length = sizeof (cli_addr);

        write(1, "$Jack:\n", sizeof("$Jack:\n"));
        write(1, "Waiting...\n", sizeof("Waiting...\n"));

        socketsClients[socketCounter] = accept (generalSocketFD, (void *) &cli_addr, &length);
        if (socketsClients[socketCounter] < 0){
            bytes = sprintf(buff, ACCEPT_ERROR);
            write(1, buff, bytes);
            return -1;
        }

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
                //TODO: Afegir el nom del client
                char *nomclient;
                nomclient = protocolconnexioServidor(socketsClients[socketCounter]);
                if (strcmp(nomclient, "ERROR") == 0) return 0;
                write(1, NEW_CONNECTION, strlen(NEW_CONNECTION));
                write(1, nomclient, strlen(nomclient));
                write(1, "\n", sizeof("\n"));
                gestionarClient(socketsClients[socketCounter], nomclient);
                return 0;
                break;

            default:
                //Pare
                forkCounter++;
                socketCounter++;
                socketsClients = (int *) realloc(socketsClients,(forkCounter+1)*sizeof(int));
                forkIDsClients = (int *) realloc(forkIDsClients,(socketCounter+1)*sizeof(int));
                break;
        }

    }
    return 0;
}
