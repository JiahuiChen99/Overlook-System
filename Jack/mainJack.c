#define _POSIX_SOURCE
#include <pthread.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/shm.h>

#include "../connectionUtils/socket.h"
#include "../fileParser.h"
#include "../Lloyd/lloyd.h"

//strings
#define START  "Starting Jack...\n"
#define ERROR_FORK "Error al fer fork! \n"
#define NEW_CONNECTION "New Connection: "
#define SEM_CREATE_ERROR "Error en crear el semàfor \n"
#define ERR_OUT -1

//Lloyd
#define ERROR_FORK_LLOYD "Error al fer fork! \n"

//Variables globals
int generalSocketFD;
int socketTemp;
int *forkIDsClients;
int forkCounter = 0;
pid_t parent_pid;

//Variables globals per conectar amb Lloyd
int lloydID;
semaphore semFills;
Sincronitzacio sincron;
int shm;
infoLloyd * memComp;

int getMemoriaCompartida(){
    shm = shmget(ftok("../Lloyd/lloyd.c", 'a'), sizeof(infoLloyd), 0600);
    if(shm < 0){
        perror("shmget");
        return ERR_OUT;
    }
    //shmat
    memComp = (infoLloyd *)shmat(shm, 0, 0);
    if (memComp == NULL){
        perror("shmat");
        return ERR_OUT;
    }else{
        memset(memComp->nomEstacio, '\0', 101);
        memComp->temperatura         = 0;
        memComp->humitat             = 0;
        memComp->pressio_atmosferica = 0;
        memComp->precipitacio        = 0;
    }
    return 0;
}

int inicialitzaSemafors(){
    int s = SEM_constructor_with_name(&(sincron.semJack), ftok("../Lloyd/lloyd.c", 'J'));

    if (s < 0){
        write(1, SEM_CREATE_ERROR, sizeof(SEM_CREATE_ERROR));
        return ERR_OUT;
    }

    s = SEM_constructor_with_name(&(sincron.semJack2), ftok("../Lloyd/lloyd.c", 'P'));

    if (s < 0){
        write(1, SEM_CREATE_ERROR, sizeof(SEM_CREATE_ERROR));
        return ERR_OUT;
    }

    s = SEM_constructor_with_name(&semFills, ftok("mainJack.c", 'F'));
    if (s < 0){
        write(1, SEM_CREATE_ERROR, sizeof(SEM_CREATE_ERROR));
        return ERR_OUT;
    }

    s = SEM_init(&semFills, 1);
    if (s < 0){
        write(1, SEM_CREATE_ERROR, sizeof(SEM_CREATE_ERROR));
        return ERR_OUT;
    }

    return 0;
}

void destrueixSemafors(){
    //Destruim semàfor
    SEM_destructor(&(sincron.semJack));
    SEM_destructor(&(sincron.semJack2));
    SEM_destructor(&semFills);
}

void signalhandler(int sigint){

    switch (sigint) {
        case SIGINT:
            //La interrupció es propaga a tots els fills
            if(parent_pid == getpid()){
                for (int i = 0; i < forkCounter; i++) {
                    kill(forkIDsClients[i], SIGUSR1);
                }

                kill(lloydID, SIGINT);
                free(forkIDsClients);
                close(generalSocketFD);
                shutdown(generalSocketFD, 2);
                close(socketTemp);
                shutdown(socketTemp, 2);
            }else{
                free(forkIDsClients);
                close(socketTemp);
                shutdown(socketTemp, 2);
            }

            //Tanquem semàfors
            destrueixSemafors();
            //Netejem la memoria compartida
            shmdt((*memComp).nomEstacio);
            shmdt(memComp);

            exit(0);
            break;
        case SIGUSR1: //Alliberar recursos de cada fill
            free(forkIDsClients);
            close(socketTemp);
            shutdown(socketTemp, 2);
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
    llegirConfig(argv[1], "Jack", NULL, &config);

    //Iniciem el servidor
    generalSocketFD = iniciarServidor(config.ipJack, config.portJack);

    //Reassingem interrupcions
    signal(SIGINT, signalhandler);

    //Agafem la memòria compartida
    int s =crearMemoriaCompartida();
    if (s < 0) {return ERR_OUT;}

    //Inicialitzem semàfors
    s = crearSemafors();
    if (s < 0) {return ERR_OUT;}

    //Crear els recursos per comunicar-se amb Lloyd
    getMemoriaCompartida();
    inicialitzaSemafors();


    //Creació de Lloyd
    lloydID = fork();
    switch (lloydID) {
        case -1:
            bytes = sprintf(buff, ERROR_FORK_LLOYD);
            write(1, buff, bytes);
            return -1;
            break;
        case 0:   //Lloyd
            ;
            int lloydStatus = processaLloyd();
            if(lloydStatus < 0){
                return -1;
            }
            return 0;
            break;
        default:  //Pare
            forkIDsClients = (int *) malloc(sizeof(int));
            //Esperem a rebre connexions
            while(1){
                write(1, "$Jack:\n", sizeof("$Jack:\n"));
                write(1, "Waiting...\n", sizeof("Waiting...\n"));

                socketTemp = acceptarConnexio(generalSocketFD);
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

                        char *nomclient;
                        nomclient = protocolconnexioServidor(socketTemp);
                        if (strcmp(nomclient, "ERROR") == 0) return -1;
                        write(1, NEW_CONNECTION, strlen(NEW_CONNECTION));
                        write(1, nomclient, strlen(nomclient));
                        write(1, "\n", sizeof("\n"));
                        free(nomclient);
                        gestionarClient(socketTemp, sincron, semFills, memComp);
                        return 0;
                        break;

                    default:
                        //Pare
                        forkCounter++;
                        break;
                }
            }
            break;
    }
    return 0;
}
