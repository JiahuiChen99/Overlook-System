#define _POSIX_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>

#include "fileParser.h"
#include "./connectionUtils/socket.h"

#define START "Starting Danny...\n"
#define DISCONNECTION "Disconnecting Danny...\n"
#define DISCONNECTION_JACK "Disconnecting Jack...\n"
#define DISCONNECTION_WENDY "Disconnecting Wendy...\n"

//Variables globals
configDanny config;
int fdsocket;
int socketW;
int pidJack;
int pidWendy;
int fdFitxer;

void signalhandler(int sigint){

    switch (sigint) {

        case SIGALRM:
            //Comprovar la carpeta i mirar si tenim arxius de text
            fileDetection(&config, fdsocket, socketW, fdFitxer);

            break;
        case SIGINT:
            //Avisar Jack
            protocolDesconnexio(fdsocket, config.nom);
            close(fdsocket);
            shutdown(fdsocket, 2);

            free (config.nom);
            free (config.carpeta);
            free (config.ipJack);
            free (config.ipWendy);

            write(1, DISCONNECTION, sizeof(DISCONNECTION));
            exit(0);
            break;
        case SIGPIPE:

            close(fdsocket);
            shutdown(fdsocket, 2);

            free (config.nom);
            free (config.carpeta);
            free (config.ipJack);
            free (config.ipWendy);

            write(1, DISCONNECTION_JACK, sizeof(DISCONNECTION_JACK));
            kill(pidJack, SIGINT);
            write(1, DISCONNECTION, sizeof(DISCONNECTION));
            kill(pidWendy, SIGINT);
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

    if(argc < 2){
        char buff[100];
        int bytes = sprintf(buff, ARGUMENT_ERROR);
        write(1, buff, bytes);
        exit(ERROR_RETURN);
    }

    write(1, START, sizeof(START));

    config.nom = NULL;
    config.carpeta = NULL;
    config.temps = 0;
    config.ipJack = NULL;
    config.portJack = 0;
    config.ipWendy = NULL;
    config.portWendy = 0;

    llegirConfig(argv[1], "Danny", &config, NULL);

    //Reassingem interrupcions
    signal(SIGALRM, signalhandler);
    signal(SIGINT, signalhandler);
    signal(SIGPIPE, signalhandler);

    //Iniciem la conexió amb el servidor Jack
    fdsocket = iniciarclient(config.ipJack, config.portJack);
    if(fdsocket < 0){
        //Sortir
        exit -1;
    }
    pidJack = protocolconnexioClient(fdsocket, config.nom);

    //Iniciem la conexió amb el servidor Wendy
    socketW = iniciarclient(config.ipWendy, config.portWendy);
    pidWendy = protocolconnexioClient(socketW, config.nom);
    if(socketW < -1){
        //Display error i sortir
        return -1;
    }

    while(1){
        /*sleep(config.temps);
        raise(SIGALRM);*/
        alarm(config.temps);
        pause();
    }

    return 0;
}
