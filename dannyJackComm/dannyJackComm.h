#ifndef OVERLOOK_SYSTEM_SOCKETS_H
#define OVERLOOK_SYSTEM_SOCKETS_H

//Defines necessaris
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include "../semaphore_v2.h"

typedef struct {
    char * nomEstacio;
    float temperatura;
    int humitat;
    float pressio_atmosferica;
    float precipitacio;
} infoLloyd;

typedef struct {
    char * data;
    char * hora;
    float temperatura;
    int humitat;
    float pressio_atmosferica;
    float precipitacio;
} txtFile;

typedef struct {
    char origen[14];
    char tipus;
    char dades[100];
} osPacket;

//Sockets utils
//Cridar desde mainJack.c
int gestionarClient(int fd, char *nomclient, semaphore semJack, semaphore semFills, infoLloyd * memComp);


//Jack rebent Danny
//Es crida desde gestionarClient
int parseigDadesDanny(osPacket dadesMeteorologiques, semaphore semJack, semaphore semFills, infoLloyd * memComp);
int llegirDadesClient(int fd, char *nomclient, semaphore semJack, semaphore semFills, infoLloyd * memComp);

//Danny enviant a Jack
//Es crida desde fileParser
int enviarDadesClient(int socketFD, txtFile txtFile, configDanny *config);

//Utils
//Es crida desde aquí dins
void dadesMeteorologiquesSerializer(char *serial, char *dades);
//Es crida desde parseigDadesDanny
void enviarALloyd(txtFile dadesDanny, char * origen, semaphore semJack, semaphore semFills, infoLloyd * memComp);

#endif
