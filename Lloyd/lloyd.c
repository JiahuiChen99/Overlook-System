#include "lloyd.h"

//Define strings
#define START "Starting Lloyd...\n"
#define DISCONNECTION "Disconnecting Lloyd...\n"
#define SEM_CREATE_ERROR "Error en crear el semàfor \n"

#define ERR_OUT -1

Sincronitzacio sincron;
int shm;
infoLloyd * memComp;
infoLloyd * estacions; //Array de les mitjanes de estacions
infoLloyd * infoAcumulada; //Matriu on guardem totes les instàncies de info que ens arriben
int * numDades; //Guardem quantes dades té cada array de infoAcumulada
int numEstacions;

//Signal Handler per a controlar ctrl+c que faràn sortir el programa
void signalhandlerLloyd(int signum){
    char buff[100];
    int nBytes = 0;
    int out;

    switch(signum){
        case SIGALRM:
            //Guardar la informació a memòria secundària
            printf("LA ALARMA HA SONAT EN 20 SEC\n");
            out = open("Hallorann.txt", O_RDWR | O_CREAT, 0666);

            //Per cada estació
            for (int i = 0; i < numEstacions; i++) {
                write(out, estacions[i].nomEstacio, strlen(estacions[i].nomEstacio));
                write(out, "\n", 1);

                nBytes = sprintf(buff, "%.2f\n", estacions[i].temperatura);
                write(out, buff, nBytes);

                nBytes = sprintf(buff, "%d\n", estacions[i].humitat);
                write(out, buff, nBytes);

                nBytes = sprintf(buff, "%.2f\n", estacions[i].pressio_atmosferica);
                write(out, buff, nBytes);

                nBytes = sprintf(buff, "%.2f\n", estacions[i].precipitacio);
                write(out, buff, nBytes);
            }

            alarm(20);
            break;

        case SIGINT:
            //Destruim semàfor
            SEM_destructor(&(sincron).semJack);
            SEM_destructor(&(sincron).semJack2);
            //Netejem la memoria compartida
            shmdt(memComp);
            shmctl(shm, IPC_RMID, NULL);

            //Alliberem la memòria reservada.
            if(numEstacions > 0){
                free(estacions);
                free(infoAcumulada);
                free(numDades);
            }
            exit(0);
            break;
        default:
            break;
    }
    signal(signum, signalhandlerLloyd);
    //return 0;
}

int buscaEstacio(char * estacio, int numEstacions){
    for (int i = 0; (i < numEstacions); i++){
        if(strcmp(estacio, estacions[i].nomEstacio) == 0){
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
    strcpy(estacions[estacio].nomEstacio, memComp->nomEstacio);
    estacions[estacio].temperatura = memComp->temperatura;
    estacions[estacio].humitat = memComp->humitat;
    estacions[estacio].pressio_atmosferica = memComp->pressio_atmosferica;
    estacions[estacio].precipitacio = memComp->precipitacio;
}

void guardaDadesAcumulades(int estacio){
    strcpy(infoAcumulada[estacio].nomEstacio, memComp->nomEstacio);
    infoAcumulada[estacio].temperatura += memComp->temperatura;
    infoAcumulada[estacio].humitat += memComp->humitat;
    infoAcumulada[estacio].pressio_atmosferica += memComp->pressio_atmosferica;
    infoAcumulada[estacio].precipitacio += memComp->precipitacio;
}

void inicialitzaAcum(int index){
    memset(infoAcumulada[index].nomEstacio, '\0', 101);
    infoAcumulada[index].temperatura = 0;
    infoAcumulada[index].humitat = 0;
    infoAcumulada[index].pressio_atmosferica = 0;
    infoAcumulada[index].precipitacio = 0;
}

int crearMemoriaCompartida() {
    //shmget  struct
    shm = shmget(ftok("../Lloyd/lloyd.c", 'a'), sizeof(infoLloyd), IPC_CREAT | IPC_EXCL | 0600);
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

    return 0;
}

int crearSemafors(){
    //Inicialitzem semàfors
    int s = 0;
    s = SEM_constructor_with_name(&(sincron.semJack), ftok("../Lloyd/lloyd.c",'J'));

    if (s < 0){
        write(1, SEM_CREATE_ERROR, sizeof(SEM_CREATE_ERROR));
        return ERR_OUT;
    }

    s = SEM_init(&(sincron).semJack, 0);
    if (s < 0){
        write(1, SEM_CREATE_ERROR, sizeof(SEM_CREATE_ERROR));
        return ERR_OUT;
    }

    s = SEM_constructor_with_name(&(sincron.semJack2), ftok("../Lloyd/lloyd.c",'P'));

    if (s < 0){
        write(1, SEM_CREATE_ERROR, sizeof(SEM_CREATE_ERROR));
        return ERR_OUT;
    }

    s = SEM_init(&(sincron.semJack2), 0);
    if (s < 0){
        write(1, SEM_CREATE_ERROR, sizeof(SEM_CREATE_ERROR));
        return ERR_OUT;
    }

    return 0;
}

int processaLloyd(){
    numEstacions = 0;
    int sem_status = 0;

    //Reassignem signals
    signal(SIGINT, signalhandlerLloyd);
    signal(SIGALRM, signalhandlerLloyd);

    alarm(20);

    while (1) {
        do{
            sem_status = SEM_wait(&(sincron.semJack));
            printf("ALARMA: sem_status: %d\n", sem_status);
        }while(sem_status < 0);

        printf("------------- ENTRA\n");
        //Guardem info i calculem mitjana
        if (numEstacions != 0) {

            /*Comprovem si tenim l'estació si ja existeix afegir informació
              sinó afegim un nou espai*/
            int estacio = buscaEstacio(memComp->nomEstacio, numEstacions);
            if (estacio == -1){
                //Creem una nova
                numEstacions++; //Augmentem el nombre de estacions existents
                estacions = (infoLloyd *) realloc(estacions, sizeof(infoLloyd) * numEstacions); //Reservem espai per la nova estacio
                infoAcumulada = (infoLloyd *) realloc(infoAcumulada, sizeof(infoLloyd) * numEstacions); //Reservem espai per la nova estacio a l'acumulat
                numDades = (int *) realloc(numDades, sizeof(int) * numEstacions); //Reservo espai per el nombre de dades/estacio
                numDades[numEstacions-1] = 1;
                //Inicialitzar infoAcumulada
                inicialitzaAcum(numEstacions-1);
                //Afegim les noves dades a la estacio
                guardaDadesAcumulades(numEstacions-1);
                guardaDadesMitjana(numEstacions-1);
            }else{
                //Fer un guardat dades acumulades
                numDades[estacio]++;
                infoAcumulada = (infoLloyd *) realloc(infoAcumulada, sizeof(infoLloyd)*numDades[estacio]);
                guardaDadesAcumulades(estacio);
                calculaMitjana(estacio);
            }
        }else{

            //Reservem memòria per les estructures necessàries
            estacions = (infoLloyd *) malloc(sizeof(infoLloyd)); //Reservem espai per la primera estacio
            infoAcumulada = (infoLloyd *) malloc(sizeof(infoLloyd)); //Reservem espai per la primera estacio a l'acumulat
            numDades = (int *) malloc(sizeof(int)); //Reservem espai per el nombre de dades per estacio
            numDades[0] = 1;
            numEstacions++;


            printf("- Nom estacio: %s\n", memComp->nomEstacio);
            printf("- Temperatura: %.2f\n", memComp->temperatura);
            printf("- Humitat: %d\n", memComp->humitat);
            printf("- Pressio: %.2f\n", memComp->pressio_atmosferica);
            printf("- Precipitacio: %.2f\n", memComp->precipitacio);


            //Inicialitzar infoAcumulada
            inicialitzaAcum(numEstacions-1);
            //Guardem les dades
            guardaDadesAcumulades(numEstacions-1);
            guardaDadesMitjana(numEstacions-1);
        }
        SEM_signal(&(sincron.semJack2));
    }

    return 0;
}
