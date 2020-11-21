#include "socket.h"

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

    //if (inet_aton (ip, &s_addr.sin_addr) == 0){
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

int gestionarClient(int fd, char *nomclient){
    int finish=1;
    do{
        finish = llegirDadesClient(fd, nomclient);
    }while(finish >= 0);
    return 0;
}

int protocolDesconnexio(int fd, char * nom){
    char serial[115];
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
    }
    return 0;
}

char * protocolconnexioServidor(int fd){
    char serial[115];
    osPacket tramaRebuda, tramaResposta;
    int tamany = 0;
    //Inicialització de la trama
    memset(tramaRebuda.origen, '\0', sizeof(tramaRebuda.origen));
    tramaRebuda.tipus = '\0';
    memset(tramaRebuda.dades, '\0', sizeof(tramaRebuda.dades));

    //Inicialització de la tramaResposta
    memset(tramaResposta.origen, '\0', sizeof(tramaResposta.origen));
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
        tamany = sizeof("CONNEXIO OK");
        strncpy(tramaResposta.dades, "CONNEXIO OK", tamany);
        memset(serial, '\0', sizeof(serial));
        tamany = sizeof(tramaResposta.origen);
        strncpy(serial, tramaResposta.origen, tamany);
        serial[14]='O';
        dadesMeteorologiquesSerializer(serial, tramaResposta.dades);
        write(fd, serial, 115);
        char *out = tramaRebuda.dades;
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
}
