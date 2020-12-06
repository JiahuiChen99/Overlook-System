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
#include "../structs.h"

//Define strings
#define START "Starting Lloyd...\n"
#define DISCONNECTION "Disconnecting Lloyd...\n"
#define SEM_CREATE_ERROR "Error en crear el semàfor \n"

#define ERR_OUT -1
//Variables globals
semaphore semJack;
int shm, shmNom;
infoLloyd * memComp;
char ** nomMemComp; //Asteric 1 -> Memoria compartida. Asterisc 2 -> Array dinàmic
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
            shmdt(nomMemComp);
            shmctl(shmNom, IPC_RMID, NULL);
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
        if(strcmp(estacio, *estacions[i].nomEstacio) == 0){
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
    printf("LLOYD: Guardant dades mitjana\n");
    *estacions[estacio].nomEstacio = (char *) malloc(strlen(*nomMemComp) + 1);
    printf("1\n");
    memset(*estacions[estacio].nomEstacio, '\0', strlen(*nomMemComp) + 1);
    printf("2\n");
    strcpy(*estacions[estacio].nomEstacio, *nomMemComp);
    printf("3\n");
    estacions[estacio].temperatura = memComp->temperatura;
    estacions[estacio].humitat = memComp->humitat;
    estacions[estacio].pressio_atmosferica = memComp->pressio_atmosferica;
    estacions[estacio].precipitacio = memComp->precipitacio;
    printf("LLOYD: Guardant dades mitjana\n");
}

void guardaDadesAcumulades(int estacio){
    //TODO:Fer-ho amb strcpy
    printf("LLOYD: Guardant dades acumulades\n");
    *infoAcumulada[estacio].nomEstacio = (char *) malloc(strlen(*nomMemComp) + 1);
    memset(infoAcumulada[estacio].nomEstacio, '\0', strlen(*nomMemComp) + 1);
    strcpy(*infoAcumulada[estacio].nomEstacio, *nomMemComp);
    infoAcumulada[estacio].temperatura += memComp->temperatura;
    infoAcumulada[estacio].humitat += memComp->humitat;
    infoAcumulada[estacio].pressio_atmosferica += memComp->pressio_atmosferica;
    infoAcumulada[estacio].precipitacio += memComp->precipitacio;
    printf("LLOYD: Hem acabat de guardar dades acumulades\n");
}

void inicialitzaAcum(int index){
    printf("LLOYD: Iniciem acumulades\n");
    infoAcumulada[index].nomEstacio = NULL;
    infoAcumulada[index].temperatura = 0;
    infoAcumulada[index].humitat = 0;
    infoAcumulada[index].pressio_atmosferica = 0;
    infoAcumulada[index].precipitacio = 0;
    printf("LLOYD: Acabat iniciar acumulades\n");
}

int crearMemoriaCompartida() {
  //shmget  struct
  shm = shmget(ftok("lloyd.c", 'a'), sizeof(infoLloyd), IPC_CREAT | IPC_EXCL | 0600);
  if(shm < 0){
      perror("shmget");
      return ERR_OUT;
  }
  //shmat struct
  memComp = shmat(shm, 0, 0);
  if (memComp == NULL){
      perror("shmat");
      return ERR_OUT;
  }

  //shmget  nom
  shmNom = shmget(ftok("lloyd.c", 'b'), sizeof(char *), IPC_CREAT | IPC_EXCL | 0600);
  if(shm < 0){
      perror("shmget");
      return ERR_OUT;
  }
  //shmat nom
  *nomMemComp = shmat(shmNom, 0, 0);
  if (nomMemComp == NULL){
      perror("shmat");
      return ERR_OUT;
  }

  return 0;
}

int main(){
    numEstacions = 0;
    //Agafem la memòria compartida
    int s =crearMemoriaCompartida();
    if (s < 0) {return ERR_OUT;}

    //Inicialitzem semàfors
    s = SEM_constructor_with_name(&semJack, ftok("lloyd.c",'J'));

    if (s < 0){
        write(1, SEM_CREATE_ERROR, sizeof(SEM_CREATE_ERROR));
        return ERR_OUT;
    }

    s = SEM_init(&semJack, 0);
    if (s < 0){
        write(1, SEM_CREATE_ERROR, sizeof(SEM_CREATE_ERROR));
        return ERR_OUT;
    }

    //Reassignem signals
    signal(SIGINT, signalhandler);

    while (1) {
        printf("LLOYD: Esperem les dades de Jack\n");
        SEM_wait(&semJack);
        //Guardem info i calculem mitjana.
        //Comprovem si tenim l'estació
        if (numEstacions != 0) {
            int estacio = buscaEstacio(*memComp->nomEstacio, numEstacions);
            if (estacio == -1){
                //Creem una nova
                numEstacions++; //Augmentem el nombre de estacions existents
                estacions = (infoLloyd *) realloc(estacions, sizeof(infoLloyd)*numEstacions); //Reservem espai per la nova estacio
                infoAcumulada = (infoLloyd *) realloc(infoAcumulada, sizeof(infoLloyd) * numEstacions); //Reservem espai per la nova estacio a l'acumulat
                numDades = (int *) realloc(numDades, sizeof(int)*numEstacions); //Reservo espai per el nombre de dades/estacio
                numDades[numEstacions-1] = 1;
                estacions[numEstacions-1].nomEstacio = (char **) malloc(sizeof(char *));
                infoAcumulada[numEstacions-1].nomEstacio = (char **) malloc(sizeof(char *));
                //Inicialitzar infoAcumulada
                inicialitzaAcum(numEstacions-1);
                //Afegim les noves dades a la estacio
                guardaDadesAcumulades(numEstacions-1);
                guardaDadesMitjana(numEstacions-1);
            }else{
                //Fer un guarda dades acumulades
                numDades[estacio]++;
                infoAcumulada = (infoLloyd *) realloc(infoAcumulada, sizeof(infoLloyd)*numDades[estacio]);
                infoAcumulada[numDades[estacio]-1].nomEstacio = (char **) malloc(sizeof(char *));
                guardaDadesAcumulades(estacio);
                calculaMitjana(estacio);
            }
        }else{
            //Reservem memòria per les estructures necessàries

            printf("LLOYD: Hem rebut les dades de Jack 1\n");
            estacions = (infoLloyd *) malloc(sizeof(infoLloyd)); //Reservem espai per la primera estacio
            infoAcumulada = (infoLloyd *) malloc(sizeof(infoLloyd)); //Reservem espai per la primera estacio a l'acumulat
            numDades = (int *) malloc(sizeof(int)); //Reservem espai per el nombre de dades per estacio
            numDades[0] = 1;
            numEstacions++;
            estacions[numEstacions-1].nomEstacio = (char **) malloc(sizeof(char *));
            infoAcumulada[numEstacions-1].nomEstacio = (char **) malloc(sizeof(char *));

            //printf("HO GUARDEM A LA REGIO %d\n", memComp);
            printf("- Llargada nom estacio: %ld\n", strlen(*nomMemComp));
            printf("- Nom estacio: %s\n", *nomMemComp);
            printf("- Temperatura: %.2f\n", memComp->temperatura);
            printf("- Humitat: %d\n", memComp->humitat);
            printf("- Pressio: %.2f\n", memComp->pressio_atmosferica);
            printf("- Precipitacio: %.2f\n", memComp->precipitacio);

            //Inicialitzar infoAcumulada
            inicialitzaAcum(numEstacions-1);
            //Guardem les dades
            guardaDadesAcumulades(numEstacions-1);
            guardaDadesMitjana(numEstacions-1);
            printf("LLOYD: Hem tractat les dades de Jack 1\n");
        }
        SEM_signal(&semJack);
    }

    return 0;
}
