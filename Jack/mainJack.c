#define _POSIX_SOURCE
#include <pthread.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/shm.h>

#include "../connectionUtils/socket.h"
#include "../fileParser.h"

//strings
#define START  "Starting Jack...\n"
#define ERROR_FORK "Error al fer fork! \n"
#define NEW_CONNECTION "New Connection: "
#define SEM_CREATE_ERROR "Error en crear el semàfor \n"
#define ERR_OUT -1

//Variables globals
int generalSocketFD;
int socketTemp;
int *forkIDsClients;
int forkCounter = 0;
pid_t parent_pid;
//Variables globals per conectar amb Lloyd
semaphore semFills;
semaphore semJack;
int shm;
infoLloyd * memComp;

int getMemoriaCompartida(){
    shm = shmget(ftok("lloyd.c", 'a'), sizeof(infoLloyd), IPC_CREAT | IPC_EXCL | 0600);
    if(shm < 0){
        perror("shmget");
        return ERR_OUT;
    }
    //shmat
    memComp = shmat(shm, 0, 0);
    if (memComp == NULL){
        perror("shmat");
        return ERR_OUT;
    }
    return 0;
}

int inicialitzaSemafors(){
    int s = SEM_constructor_with_name(&semJack, 'J');
    if (s == 0){
        write(1, SEM_CREATE_ERROR, sizeof(SEM_CREATE_ERROR));
        return ERR_OUT;
    }

    s = SEM_constructor_with_name(&semFills, 'F');
    if (s == 0){
        write(1, SEM_CREATE_ERROR, sizeof(SEM_CREATE_ERROR));
        return ERR_OUT;
    }

    s = SEM_init(&semFills, 1);
    if (s == 0){
        write(1, SEM_CREATE_ERROR, sizeof(SEM_CREATE_ERROR));
        return ERR_OUT;
    }

    return 0;
}

void destrueixSemafors(){
    //Destruim semàfor
    SEM_destructor(&semJack);
    SEM_destructor(&semFills);
}

void signalhandler(int sigint){

    switch (sigint) {
        case SIGINT:
            //La interrupció es propaga a tots els fills
            if(parent_pid == getpid()){
                close(socketTemp);
                shutdown(socketTemp, 2);
                printf("MATO PARE\n");
            }else{
                free(forkIDsClients);
                close(generalSocketFD);
                shutdown(generalSocketFD, 2);
                printf("MATO FILL\n");
            }

            //Tanquem semàfors
            destrueixSemafors();
            //Netejem la memoria compartida
            shmdt(memComp);
            shmctl(shm, IPC_RMID, NULL);

            //Tanquem el servidor
            exit(0);
            break;
        default:
            for(int i = 0; i < (int)((int)sizeof(forkIDsClients)/(int)sizeof(int)); i++){
                //Tanquem els canals de comunicació - sockets
                printf("TANQUEM ELS SOCKETS en els fills\n");
                //close(socketsClients[i]);
                //shutdown(socketsClients[i], 2);
            }
            close(generalSocketFD);
            shutdown(generalSocketFD, 2);
            exit(0);
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

    forkIDsClients = (int *) malloc(sizeof(int));

    //Reassingem interrupcions
    signal(SIGINT, signalhandler);

    //Crear els recursos per comunicar-se amb Lloyd
    getMemoriaCompartida();
    inicialitzaSemafors();
    //Esperem a rebre

    while(1){
        //Fer un accept

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
                //TODO: Afegir el nom del client
                char *nomclient;
                nomclient = protocolconnexioServidor(socketTemp);
                if (strcmp(nomclient, "ERROR") == 0) return 0;
                write(1, NEW_CONNECTION, strlen(NEW_CONNECTION));
                write(1, nomclient, strlen(nomclient));
                write(1, "\n", sizeof("\n"));
                gestionarClient(socketTemp, semJack, semFills, memComp);
                //TODO: Allibrerar nomclient
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
