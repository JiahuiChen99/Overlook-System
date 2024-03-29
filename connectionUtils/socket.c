#include "socket.h"

//Jack <-> Danny
int gestionarClient(int fd, Sincronitzacio sincron, semaphore semFills, infoLloyd * memComp){
    int finish=1;
    do{
        finish = llegirDadesClient(fd, sincron, semFills, memComp);
    }while(finish >= 0);
    return 0;
}

int llegirDadesClient(int socketFD, Sincronitzacio sincron, semaphore semFills, infoLloyd * memComp){
    char trama[sizeof(osPacket)], serial[115];
    osPacket dadesMeteorologiques, tramaResposta;

    int nBytes = read(socketFD, trama, sizeof(osPacket));
    write(1, "$Jack:\n", sizeof("$Jack:\n"));
    write(1, RECEIVE_DATA, strlen(RECEIVE_DATA));

    //Inicialització del serial
    memset(serial, '\0', sizeof(serial));
    //Inicialització de la trama
    memset(tramaResposta.origen, '\0', sizeof(tramaResposta.origen));
    tramaResposta.tipus = '\0';
    memset(tramaResposta.dades, '\0', sizeof(tramaResposta.dades));


    //Control dels bytes rebuts, si es perden bytes durant la comunicació error
    if(nBytes != sizeof(osPacket) || trama[14] != 'D'){
        if(trama[14] == 'Q'){
            write(1, "Disconnecting Danny\n", sizeof("Disconnecting Danny\n"));
            exit(0);
        }

        //Popular la trama amb ERROR Trama
        strcpy(tramaResposta.origen, "JACK");
        tramaResposta.tipus = 'Z';
        strcpy(tramaResposta.dades, "ERROR DE TRAMA");

    }else{

        /** Lectura de trama **/
        //Lectura de l'origen
        strncpy(dadesMeteorologiques.origen, trama, 14);
        write(1, dadesMeteorologiques.origen, strlen(dadesMeteorologiques.origen));
        write(1, "\n", 1);
        //Lectura tipus
        dadesMeteorologiques.tipus = trama[14];
        //Lectura de dades
        strncpy(dadesMeteorologiques.dades, trama + 15, 100);

        int dadesStatus = parseigDadesDanny(dadesMeteorologiques, sincron, semFills, memComp);

        if(dadesStatus == ERROR_DADES_DANNY){
            //Popular la trama amb KO
            strcpy(tramaResposta.origen, "JACK");
            tramaResposta.tipus = 'K';
            strcpy(tramaResposta.dades, "DADES KO");
        }else{
            //Popular la trama amb OK
            strcpy(tramaResposta.origen, "JACK");
            tramaResposta.tipus = 'B';
            strcpy(tramaResposta.dades, "DADES OK");
        }
    }

    //Serialitzar
    memcpy(serial, tramaResposta.origen, sizeof(tramaResposta.origen));
    serial[14] =  tramaResposta.tipus;
    dadesMeteorologiquesSerializer(serial, tramaResposta.dades);

    //Enviar
    write(socketFD, serial, sizeof(serial));


    return 0;
}

int parseigDadesDanny(osPacket dadesMeteorologiques, Sincronitzacio sincron, semaphore semFills, infoLloyd * memComp){
    int i, j, hashtagCounter, bytes;
    char *aux = NULL, buff[100];
    i = j = bytes = hashtagCounter = 0;
    txtFile dadesDanny;

    aux = (char *) malloc(sizeof(char) * 1);
    memset(buff, '\0', sizeof(buff));


    dadesDanny.data = NULL;
    dadesDanny.hora = NULL;
    dadesDanny.temperatura = 0.00f;
    dadesDanny.humitat = 0;
    dadesDanny.pressio_atmosferica = 0.00f;
    dadesDanny.precipitacio = 0.00f;

    while(i < 100){
        while(dadesMeteorologiques.dades[i] != '#' && i < 100){
            if(hashtagCounter < 5){
                aux[j] = dadesMeteorologiques.dades[i];
                j++;
                aux = (char *)realloc(aux, sizeof(char)* (j + 1));
                i++;
            }else{
                if(dadesMeteorologiques.dades[i] != '\0'){
                    aux[j] = dadesMeteorologiques.dades[i];
                    j++;
                    aux = (char *)realloc(aux, sizeof(char)* (j + 1));
                    i++;
                }else{
                    break;
                }
            }
        }

        //Recordar posar \0 SEMPRE
        aux[j] = '\0';
        switch (hashtagCounter) {
            case 0:
                //Comprovar data
                if(strlen(aux) != 10){
                    free(aux);
                    return ERROR_DADES_DANNY;
                }
                dadesDanny.data = (char *)malloc(sizeof(char)* (j + 1));
                memset(dadesDanny.data, '\0', j + 1);
                strcpy(dadesDanny.data, aux);
                break;
            case 1:
                //Comprovar hora
                if(strlen(aux) != 8){
                    free(aux);
                    return ERROR_DADES_DANNY;
                }
                dadesDanny.hora = (char *)malloc(sizeof(char)* (j + 1));
                memset(dadesDanny.hora, '\0', j + 1);
                strcpy(dadesDanny.hora, aux);
                break;
            case 2:
                //Comprovar temperatura
                if(strlen(aux) > 5){
                    free(aux);
                    return ERROR_DADES_DANNY;
                }
                dadesDanny.temperatura = atof(aux);
                break;
            case 3:
                //Comprovar humitat
                if(strlen(aux) > 3){
                    free(aux);
                    return ERROR_DADES_DANNY;
                }
                dadesDanny.humitat = atoi(aux);
                break;
            case 4:
                //Comprovar pressió
                if(strlen(aux) > 6){
                    free(aux);
                    return ERROR_DADES_DANNY;
                }
                dadesDanny.pressio_atmosferica = atof(aux);
                break;
            case 5:
                //Comprovar precipitació
                if(strlen(aux) > 4){
                    free(aux);
                    return ERROR_DADES_DANNY;
                }
                dadesDanny.precipitacio = atof(aux);
                break;
            default:
                break;
        }
        hashtagCounter++;
        i++;
        j = 0;
    }

    write(1, dadesDanny.data, strlen(dadesDanny.data));
    write(1, "\n", 1);
    write(1, dadesDanny.hora, strlen(dadesDanny.hora));
    write(1, "\n", 1);
    bytes = sprintf(buff, "%.1f", dadesDanny.temperatura);
    write(1, buff, bytes);
    write(1, "\n", 1);
    bytes = sprintf(buff, "%d", dadesDanny.humitat);
    write(1, buff, bytes);
    write(1, "\n", 1);
    bytes = sprintf(buff, "%.1f", dadesDanny.pressio_atmosferica);
    write(1, buff, bytes);
    write(1, "\n", 1);
    bytes = sprintf(buff, "%.1f", dadesDanny.precipitacio);
    write(1, buff, bytes);
    write(1, "\n", 1);

    //Enviar dades a Lloyd
    enviarALloyd(dadesDanny, dadesMeteorologiques.origen, sincron, semFills, memComp);


    free(dadesDanny.data);
    free(dadesDanny.hora);
    free(aux);

    return 0;
}

int enviarDadesClient(int socketFD, txtFile txtFile, configDanny *config){

    //Inicialització del serial
    char serial[115];
    char aux[10];
    memset(aux, '\0', sizeof(aux));
    memset(serial, '\0', sizeof(serial));
    //Inicialització de la trama
    osPacket message;
    memset(message.origen, '\0', sizeof(message.origen));
    message.tipus = '\0';
    memset(message.dades, '\0', sizeof(message.dades));

    // enviar nom estació
    strcpy(message.origen,config->nom);
    message.tipus = 'D';
    //TODO: enviar tota la info
    int index=0;
    strcpy(message.dades,txtFile.data);
    index = strlen(message.dades);
    message.dades[index]='#';

    strcat(message.dades,txtFile.hora);
    index = strlen(message.dades);
    message.dades[index]='#';

    sprintf(aux, "%.1f%c", txtFile.temperatura, '\0');
    strcat(message.dades,aux);
    index = strlen(message.dades);
    message.dades[index]='#';


    sprintf(aux, "%d%c", txtFile.humitat, '\0');
    strcat(message.dades,aux);
    index = strlen(message.dades);
    message.dades[index]='#';

    sprintf(aux, "%.1f%c", txtFile.pressio_atmosferica, '\0');
    strcat(message.dades,aux);
    index = strlen(message.dades);
    message.dades[index]='#';


    sprintf(aux, "%.1f%c", txtFile.precipitacio, '\0');
    strcat(message.dades,aux);
    index = strlen(message.dades);
    message.dades[index]='\0';


    //Serialitzar
    memcpy(serial, message.origen, sizeof(message.origen));
    serial[14] =  message.tipus;
    dadesMeteorologiquesSerializer(serial, message.dades);


    int error = 0;
    socklen_t len = sizeof (error);
    getsockopt (socketFD, SOL_SOCKET, SO_ERROR, &error, &len);

    write(socketFD, serial, 115);

    //Esperem la resposta i la examinem
    read(socketFD, serial, 115);
    osPacket resposta;

    //Inicialitzem
    memset(resposta.origen, '\0', sizeof(resposta.origen));
    resposta.tipus = '\0';
    memset(resposta.dades, '\0', sizeof(resposta.dades));
    //Deserialitzem
    /** Lectura de trama **/
    //Lectura de l'origen
    strncpy(resposta.origen, serial, 4);
    //Lectura tipus
    resposta.tipus = serial[14];
    //Lectura de dades
    strncpy(resposta.dades, serial + 15, 100);
    switch(resposta.tipus){
        case 'Z':
            //Error de trames
            write(1, ERROR_DE_TRAMA, strlen(ERROR_DE_TRAMA));
            break;
        case 'K':
            //Error de dades
            write(1, ERROR_DE_DADES, strlen(ERROR_DE_DADES));
            break;
        default:
            //Tot Correcte
            break;
    }


    return 0;
}


/**
* Ens permet concatenar les dades meteològiques a l'estructura
* serialitzada que enviem cap a Jack
* RECORDATORI: Les funcions de la llibreria string funcionen fins a trobar
* el primer \0, el prints i writes també
**/
void dadesMeteorologiquesSerializer(char *serial, char *dades){
    int j = 15;

    for(int i = 0; i < 100; i++){
        serial[j] = dades[i];
        j++;
    }
}

void enviarALloyd(txtFile dadesDanny, char * origen, Sincronitzacio sincron, semaphore semFills, infoLloyd * memComp){
    //Esperem a tenir el torn a la memòria compartida

    SEM_wait(&semFills);

    strcpy(memComp->nomEstacio, origen);
    memComp->temperatura         = dadesDanny.temperatura;
    memComp->humitat             = dadesDanny.humitat;
    memComp->pressio_atmosferica = dadesDanny.pressio_atmosferica;
    memComp->precipitacio        = dadesDanny.precipitacio;

    SEM_signal(&(sincron).semJack);
    SEM_wait(&(sincron).semJack2);

    SEM_signal(&semFills);
}


//General
int iniciarServidor(char *ip, int port){
    char buff[100];
    int bytes;
    //Creem el socket
    int socketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //Preparem la estructura
    struct sockaddr_in s_addr;
    memset (&s_addr, 0, sizeof (s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons (port);

    if (inet_pton (AF_INET,ip, &s_addr.sin_addr) == 0){
        bytes = sprintf(buff, IP_ERROR, ip);
        write(1, buff, bytes);
        return -1;
    }
    //Executem el bind per indicar que esperem rebre info
    if(bind(socketFD,(void *) &s_addr, sizeof(s_addr)) < 0){
        bytes = sprintf(buff, BIND_ERROR);
        write(1, buff, bytes);
        return -1;
    }

    if(listen(socketFD, 3)){
        bytes = sprintf(buff, LISTEN_ERROR);
        write(1, buff, bytes);
        return -1;
    }
    return(socketFD);
}

int iniciarclient(char *ip, int port){
    char buff[100];
    int bytes;
    //Creem el socket
    int socketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in s_addr;
    memset (&s_addr, 0, sizeof (s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons (port);

    if (inet_pton(AF_INET,ip, &s_addr.sin_addr) == 0){
        bytes = sprintf(buff, IP_ERROR, ip);
        write(1, buff, bytes);
        return -1;
    }

    //Executem el bind per indicar que esperem rebre info
    if(connect(socketFD, (void *) &s_addr, sizeof(s_addr)) < 0){
        bytes = sprintf(buff, BIND_ERROR);
        write(1, buff, bytes);
        return -1;
    }

    return socketFD;
}

int protocolDesconnexio(int fd, char * nom){
    char serial[115];
    memset(serial, '\0', sizeof(serial));

    //Inicialització de la trama
    osPacket message;
    memset(message.origen, '\0', sizeof(message.origen));
    message.tipus = '\0';
    memset(message.dades, '\0', sizeof(message.dades));

    // enviar nom estació
    strcpy(message.origen, "DANNY");

    //Serialitzar
    memcpy(serial, message.origen, sizeof(message.origen));
    serial[14] =  'Q';
    dadesMeteorologiquesSerializer(serial, nom);

    //enviem
    write(fd, serial, 115);
    return 0;
}

int protocolconnexioClient(int fd, char * nom){
    char serial[115];
    memset(serial, '\0', sizeof(serial));

    //Inicialització de la trama
    osPacket message;
    memset(message.origen, '\0', sizeof(message.origen));
    message.tipus = '\0';
    memset(message.dades, '\0', sizeof(message.dades));

    // enviar nom estació
    strcpy(message.origen, "DANNY");

    //Serialitzar
    memcpy(serial, message.origen, sizeof(message.origen));
    serial[14] =  'C';
    dadesMeteorologiquesSerializer(serial, nom);

    //enviem
    write(fd, serial, 115);

    read(fd, serial, 115);

    if (serial[14] == 'E') {
        return 1;
    }else{
        char buff[100];
        memset(buff, '\0', 100);
        strncpy(buff, serial+15, 100);
        return (atoi(buff));
    }

    return 0;
}

char * protocolconnexioServidor(int fd){
    char serial[115];
    memset(serial, '\0', sizeof(serial));

    osPacket tramaRebuda, tramaResposta;
    int tamany = 0;
    //Inicialització de la trama
    memset(tramaRebuda.origen, '\0', sizeof(tramaRebuda.origen));
    tramaRebuda.tipus = '\0';
    memset(tramaRebuda.dades, '\0', sizeof(tramaRebuda.dades));

    //Inicialització de la tramaResposta
    memset(tramaResposta.origen, '\0', sizeof(tramaResposta.origen));
    tramaResposta.tipus = '\0';
    memset(tramaResposta.dades, '\0', sizeof(tramaResposta.dades));

    //Llegim
    read(fd, serial, 115);

    /** Lectura de trama **/
    //Lectura de l'origen
    strncpy(tramaRebuda.origen, serial, 14);
    //Lectura tipus
    tramaRebuda.tipus = serial[14];
    //Lectura de dades
    strncpy(tramaRebuda.dades, serial + 15, 100);

    if ((strcmp("DANNY", tramaRebuda.origen)==0)&&(tramaRebuda.tipus == 'C')){
        tamany = sizeof("JACK");
        strncpy(tramaResposta.origen, "JACK", tamany);
        int pidpare = getppid();
        char buff[100];
        memset(buff, '\0', 100);
        tamany = sprintf(buff, "%d", pidpare);
        //tamany = sizeof("CONNEXIO OK");
        //strncpy(tramaResposta.dades, "CONNEXIO OK", tamany);
        strncpy(tramaResposta.dades, buff, tamany);
        memset(serial, '\0', sizeof(serial));
        tamany = sizeof(tramaResposta.origen);
        strncpy(serial, tramaResposta.origen, tamany);
        serial[14]='O';
        dadesMeteorologiquesSerializer(serial, tramaResposta.dades);

        write(fd, serial, 115);
        char *out = (char *)malloc(sizeof(char)* (strlen(tramaRebuda.dades) + 1));

        tamany = strlen(tramaRebuda.dades) + 1;
        memset(out, '\0', tamany);
        strcpy(out, tramaRebuda.dades);

        return(out);
    }else{
        tamany = sizeof("JACK");
        strncpy(tramaResposta.origen, "JACK", tamany);
        tamany = sizeof("ERROR");
        strncpy(tramaResposta.dades, "ERROR", tamany);
        memset(serial, '\0', sizeof(serial));
        tamany = sizeof(tramaResposta.origen);
        strncpy(serial, tramaResposta.origen, tamany);
        serial[14]='E';
        dadesMeteorologiquesSerializer(serial, tramaResposta.dades);
        write(fd, serial, 115);
        return("ERROR");
    }
    return NULL;
}

int acceptarConnexio(int generalSocketFD){
    struct sockaddr_in cli_addr;
    socklen_t length = sizeof (cli_addr);

    return accept (generalSocketFD, (void *) &cli_addr, &length);;
}

int tramaInicialWendy(int fd, char * nom, int mida, char * md5sum){
    char serial[115];
    char buff[100];
    memset(serial, '\0', sizeof(serial));

    //Inicialització de la trama
    osPacket message;
    memset(message.origen, '\0', sizeof(message.origen));
    message.tipus = '\0';
    memset(message.dades, '\0', sizeof(message.dades));

    // enviar nom estació
    strcpy(message.origen, "DANNY");

    //Serialitzar
    memcpy(serial, message.origen, sizeof(message.origen));
    serial[14] =  'I';

    //char buffDades[100];
    char midaChars[35];
    memset(buff, '\0', 100);
    memset(midaChars, '\0', 36);
    strcpy(buff, nom);
    strcat(buff, "#");
    sprintf(midaChars, "%d%c", mida, '\0');
    strcat(buff, midaChars);
    strcat(buff, "#");
    strcat(buff, md5sum);
    dadesMeteorologiquesSerializer(serial, buff);

    //enviem
    write(fd, serial, 115);

    return 0;
}

//Wendy
InfoImatge parseigTramaInicialWendy(char * dades, InfoImatge info){
    char aux[39];
    memset(aux, '\0', 39);

    int hashtagCounter = 0;
    int k, j, i;
    i = k = j = 0;

    do{
        if(dades[i] == '#'){
            hashtagCounter++;
        }

        if(hashtagCounter == 0){
            info.nom[i] = dades[i];
        }

        if(hashtagCounter == 1 && dades[i] != '#'){
            aux[k] = dades[i];
            k++;
        }

        if(hashtagCounter == 2 && dades[i] != '#'){
            info.md5[j] = dades[i];
            j++;
        }

        i++;
    }while(i < 100);

    info.mida = atoi(aux);

    return info;
}

InfoImatge llegirTramaInicial(int fd){
    InfoImatge info;
    char serial[115];
    osPacket tramaRebuda, tramaResposta;
    int tamany = 0;

    //Inicialització de la trama
    memset(tramaRebuda.origen, '\0', sizeof(tramaRebuda.origen));
    tramaRebuda.tipus = '\0';
    memset(tramaRebuda.dades, '\0', sizeof(tramaRebuda.dades));


    //Inicialització de la info
    memset(info.nom, '\0', 30);
    info.mida = 0;
    memset(info.md5, '\0', 33);

    memset(serial, '\0', sizeof(serial));

    //Inicialització de la tramaResposta
    memset(tramaResposta.origen, '\0', sizeof(tramaResposta.origen));
    tramaResposta.tipus = '\0';
    memset(tramaResposta.dades, '\0', sizeof(tramaResposta.dades));

    //Llegim
    read(fd, serial, 115);

    /** Lectura de trama **/
    //Lectura de l'origen
    strncpy(tramaRebuda.origen, serial, 14);
    //Lectura tipus
    tramaRebuda.tipus = serial[14];
    //Lectura de dades
    strncpy(tramaRebuda.dades, serial + 15, 100);

    if ((strcmp("DANNY", tramaRebuda.origen)==0)&&(tramaRebuda.tipus == 'I')){
        write(1, tramaRebuda.origen, strlen(tramaRebuda.origen));
        write(1, "\n", 1);

        info = parseigTramaInicialWendy(tramaRebuda.dades, info);

        return(info);
    }else{
        if(serial[14] == 'Q'){
            write(1, "Disconnecting Danny\n", sizeof("Disconnecting Danny\n"));
            exit(0);
        }
        tamany = sizeof("WENDY");
        strncpy(tramaResposta.origen, "WENDY", tamany);
        tamany = sizeof("ERROR");
        strncpy(tramaResposta.dades, "ERROR", tamany);
        memset(serial, '\0', sizeof(serial));
        tamany = sizeof(tramaResposta.origen);
        strncpy(serial, tramaResposta.origen, tamany);
        serial[14]='E';
        dadesMeteorologiquesSerializer(serial, tramaResposta.dades);
        write(fd, serial, 115);
        strcpy(info.nom, "ERROR");
        return(info);
    }
    strcpy(info.nom, "ERROR");
    return info;
}

int enviaBytesImatge(int fd, char * dades){
    char serial[115];
    memset(serial, '\0', sizeof(serial));

    //Inicialització de la trama
    osPacket message;
    memset(message.origen, '\0', sizeof(message.origen));
    message.tipus = '\0';
    memset(message.dades, '\0', sizeof(message.dades));

    // enviar nom estació
    strcpy(message.origen, "DANNY");

    //Serialitzar
    memcpy(serial, message.origen, sizeof(message.origen));
    serial[14] =  'F';

    dadesMeteorologiquesSerializer(serial, dades);

    //enviem
    write(fd, serial, 115);

    return 0;
}

char * repBytesImatge(int fd, int mida){
    char serial[115];
    int bytesCounter = 0;
    osPacket fragment;
    char * imatge = (char *) malloc(sizeof(char)*100);
    int bytesLlegits = 0;
    memset(imatge, '\0', 100);
    int sortir = 0;


    for(int i = 2;!sortir; i++){
        //Inicialització del serial
        memset(serial, '\0', sizeof(serial));

        int nBytes = read(fd, serial, 115);

        if(nBytes <=0)
            break;

        /** Lectura de trama **/
        //Lectura de l'origen
        strncpy(fragment.origen, serial, 14);
        //Lectura tipus
        fragment.tipus = serial[14];
        //Lectura de dades

        for(int k = 15; k < 115; k++){
            fragment.dades[k - 15] = serial[k];
        }

        /*write(1, fragment.dades, 100);
        write(1, "\n", 1);*/

        for(int k = 0; k < 100; k++){
            imatge[bytesCounter] = fragment.dades[k];
            bytesCounter++;
        }


        imatge = (char *) realloc(imatge, sizeof(char)*(100*i));
        //imatge[(100*i)+1] = '\0';
        bytesLlegits+=100;
        if (bytesLlegits >= mida)
            sortir = 1;
    }

    return(imatge);
}

int enviaSuccess(int fd){

    char serial[115];
    memset(serial, '\0', sizeof(serial));

    //Inicialització de la trama
    osPacket message;
    memset(message.origen, '\0', sizeof(message.origen));
    message.tipus = '\0';
    memset(message.dades, '\0', sizeof(message.dades));

    // enviar nom estació
    strcpy(message.origen, "WENDY");

    //Serialitzar
    memcpy(serial, message.origen, sizeof(message.origen));
    serial[14] =  'S';

    dadesMeteorologiquesSerializer(serial, "IMATGE OK");

    //enviem
    write(fd, serial, 115);

    return 0;

}

int enviaError(int fd){
    char serial[115];
    memset(serial, '\0', sizeof(serial));

    //Inicialització de la trama
    osPacket message;
    memset(message.origen, '\0', sizeof(message.origen));
    message.tipus = '\0';
    memset(message.dades, '\0', sizeof(message.dades));

    // enviar nom estació
    strcpy(message.origen, "WENDY");

    //Serialitzar
    memcpy(serial, message.origen, sizeof(message.origen));
    serial[14] =  'R';

    dadesMeteorologiquesSerializer(serial, "IMATGE KO");

    //enviem
    write(fd, serial, 115);
    return 0;
}

int comprovaMD5(InfoImatge info){
    char imagePath[100];
    memset(imagePath, '\0', 100);

    sprintf(imagePath, "./images/%s", info.nom);
    char * md5nou = NULL;
    char out[100];
    md5nou = getMD5(imagePath, out);

    if(strcmp(md5nou, info.md5) != 0){
        return -1;
    }

    return 0;
}

int gestionarClientWendy(int socketTemp){
    char imagePath[100];
    char * imatge;
    int esCorrecte;

    while(1){
        memset(imagePath, '\0', 100);
        InfoImatge info = llegirTramaInicial(socketTemp);

        if (strcmp(info.nom, "ERROR") != 0){
            imatge = repBytesImatge(socketTemp, info.mida);

            write(1, info.nom, strlen(info.nom));
            write(1, "\n", 1);

            //Escriure al fitxer de la imatge
            sprintf(imagePath, "./images/%s", info.nom);
            int fitxer = open(imagePath, O_CREAT | O_RDWR, 0666);

            write(fitxer, imatge, info.mida);
            close(fitxer);
            free(imatge);

            //Agafar i Comprovar el md5sum
            esCorrecte = comprovaMD5(info);
        }else{
            esCorrecte=-1;
        }

        if (esCorrecte == -1){
            enviaError(socketTemp);
        }else{
            enviaSuccess(socketTemp);
        }
    }

}
