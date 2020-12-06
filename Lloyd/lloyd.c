//Llibreries externes
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
#include <sys/shm.h>

//Llibreries internes
#include "../fileParser.h"
#include "../semaphore_v2.h"

//Define strings
#define START "Starting Lloyd...\n"
#define DISCONNECTION "Disconnecting Lloyd...\n"
#define SEM_CREATE_ERROR "Error en crear el semàfor \n"

#define ERR_OUT -1
//Variables globals
semaphore semJack;
int shm;
infoLloyd * memComp;
infoLloyd * estacions; //Array de les mitjanes de estacions
infoLloyd * infoAcumulada; //Matriu on guardem totes les instàncies de info que ens arriben
int * numDades; //Guardem quantes dades té cada array de infoAcumulada
int numEstacions;

//Signal Handler per a controlar ctrl+c que faràn sortir el programa
void signalhandler(int signum){
    switch(signum){
        case SIGINT:
            //Destruim semàfor
            SEM_destructor(&semJack);
            //Netejem la memoria compartida
            shmdt(memComp);
            shmctl(shm, IPC_RMID, NULL);
            //Alliberem la memòria reservada.
            for (int j = 0; j < numEstacions; j++){
                free(estacions[j].nomEstacio);
                free(infoAcumulada[j].nomEstacio);
            }
            free(estacions);
            free(infoAcumulada);
            free(numDades);
            break;
        default:
            break;
    }
    signal(signum, signalhandler);
    //return 0;
}

int buscaEstacio(char * estacio, int numEstacions){
    for (int i = 0; (i < numEstacions); i++){
        if(strcmp(estacio, estacions[i].nom) == 0){
            return (i);
        }
    }
    return -1;
}

void calculaMitjana(int estacio){
    estacions[estacio].temperatura = infoAcumulada[estacio].temperatura / numDades[estacio];
    estacions[estacio].humitat = infoAcumulada[estacio].humitat / numDades[estacio];
    estacions[estacio].pressio_atmosferica = infoAcumulada[estacio].pressio_atmosferica / numDades[estacio];
    estacions[estacio].precipitacio = infoAcumulada[estacio].precipitacio / numDades[estacio];

}

void guardaDadesMitjana(int estacio){
    //TODO:Fer-ho amb strcpy
    estacions[estacio].nomEstacio = (char *) malloc(strlen(memComp->nomEstacio));
    strcpy(infoAcumulada[estacio].nomEstacio, memComp->nomEstacio);
    estacions[estacio].temperatura = memComp->temperatura;
    estacions[estacio].humitat = memComp->humitat;
    estacions[estacio].pressio_atmosferica = memComp->pressio_atmosferica;
    estacions[estacio].precipitacio = memComp->precipitacio;
}

void guardaDadesAcumulades(int estacio){
    //TODO:Fer-ho amb strcpy
    infoAcumulada[estacio].nomEstacio = (char *) malloc(strlen(memComp->nomEstacio));
    strcpy(infoAcumulada[estacio].nomEstacio, memComp->nomEstacio);
    infoAcumulada[estacio].temperatura += memComp->temperatura;
    infoAcumulada[estacio].humitat += memComp->humitat;
    infoAcumulada[estacio].pressio_atmosferica += memComp->pressio_atmosferica;
    infoAcumulada[estacio].precipitacio += memComp->precipitacio;
}

void inicialitzaAcum(int index){
    infoAcumulada[index].temperatura = 0;
    infoAcumulada[index].humitat = 0;
    infoAcumulada[index].pressio_atmosferica = 0;
    infoAcumulada[index].precipitacio = 0;
}

int main(){
    numEstacions = 0;
    //Agafem la memòria compartida
    //shmget
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

    //Inicialitzem semàfors
    int s = SEM_constructor_with_name(&semJack, 'J');
    if (s == 0){
        write(1, SEM_CREATE_ERROR, sizeof(SEM_CREATE_ERROR));
        return ERR_OUT;
    }

    s = SEM_init(&semJack, 0);
    if (s == 0){
        write(1, SEM_CREATE_ERROR, sizeof(SEM_CREATE_ERROR));
        return ERR_OUT;
    }

    //Reassignem signals
    signal(SIGINT, signalhandler);

    while (1) {
        SEM_wait(&semJack);
        //Guardem info i calculem mitjana.
        //Comprovem si tenim l'estació
        if (numEstacions != 0) {
            int estacio = buscaEstacio(memComp->nomEstacio, numEstacions);
            if (estacio == -1){
                //Creem una nova
                numEstacions++; //Augmentem el nombre de estacions existents
                estacions = (infoLloyd *) realloc(estacions, sizeof(infoLloyd)*numEstacions); //Reservem espai per la nova estacio
                infoAcumulada = (infoLloyd *) realloc(infoAcumulada, sizeof(infoLloyd) * numEstacions); //Reservem espai per la nova estacio a l'acumulat
                numDades = (int *) realloc(numDades, sizeof(int)*numEstacions); //Reservo espai per el nombre de dades/estacio
                numDades[numEstacions-1] = 1;
                //Inicialitzar infoAcumulada
                inicialitzaAcum(numEstacions-1);
                //Afegim les noves dades a la estacio
                guardaDadesAcumulades(numEstacions-1);
                guardaDadesMitjana(numEstacions-1);
            }else{
                //Fer un guarda dades acumulades
                numDades[estacio]++;
                infoAcumulada[estacio] = (infoLloyd *) realloc(infoAcumulada, sizeof(infoLloyd)*numDades[estacio]);
                guardaDadesAcumulades(estacio);
                calculaMitjana(estacio);
            }
        }else{
            //Reservem memòria per les estructures necessàries
            estacions = (infoLloyd *) malloc(sizeof(infoLloyd)); //Reservem espai per la primera estacio
            infoAcumulada = (infoLloyd *) malloc(sizeof(infoLloyd)); //Reservem espai per la primera estacio a l'acumulat
            numDades = (int *) malloc(sizeof(int)); //Reservo espai per el nombre de dades per estacio
            numDades[0] = 1;
            numEstacions++;
            //Inicialitzar infoAcumulada
            inicialitzaAcum(numEstacions-1);
            //Guardem les dades
            guardaDadesAcumulades(numEstacions-1);
            guardaDadesMitjana(numEstacions-1);
        }
        SEM_signal(&semJack);
    }

    return 0;
}
