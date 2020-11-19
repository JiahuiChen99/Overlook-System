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

#define START "Starting Danny...\n"
#define DISCONNECTION "Disconnecting Danny...\n"

//Variables globals
configDanny config;

void signalhandler(int sigint){

    switch (sigint) {

        case SIGALRM:
            //Comprovar la carpeta i mirar si tenim arxius de text
            fileDetection(&config);

            break;
        case SIGINT:
            free (config.nom);
            free (config.carpeta);
            free (config.ipJack);
            free (config.ipWendy);


            write(1, DISCONNECTION, sizeof(DISCONNECTION));
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

    config = llegirConfig(argv[1], "Danny");

    //Reassingem interrupcions
    signal(SIGALRM, signalhandler);
    signal(SIGINT, signalhandler);

    while(1){
        sleep(config.temps);
        raise(SIGALRM);
    }

    return 0;
}
