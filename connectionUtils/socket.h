#ifndef OVERLOOK_SYSTEM_SOCKETS_H
#define OVERLOOK_SYSTEM_SOCKETS_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "../ImatgeUtils/imatgeUtils.h"
#include "../semaphore_v2.h"
#include "../structs.h"

#define IP_ERROR "%s no es una adreça IP vàlida\n"
#define BIND_ERROR "Error en fer el Bind!\n"
#define LISTEN_ERROR "Error en fer el Listen!\n"
#define ACCEPT_ERROR "Error en fer el Accept!\n"

#define ERROR_DE_TRAMA "ERROR EN EL FORMAT DE LA TRAMA!\n"
#define ERROR_DE_DADES "ERROR EN EL FORMAT DE LES DADES!\n"

//Jack - Parseig de dades Danny
#define RECEIVE_DATA "Receiving data...\n"
#define ERROR_DADES_DANNY -1

/*
*Iniciar el servidor amb la ip i port donats
*
*Returns: File Descriptor del socket
*/

//Sockets utils
//Cridar desde mainJack.c
int gestionarClient(int fd, Sincronitzacio sincron, semaphore semFills, infoLloyd * memComp);

//Jack rebent Danny
//Es crida desde gestionarClient
int parseigDadesDanny(osPacket dadesMeteorologiques, Sincronitzacio sincron, semaphore semFills, infoLloyd * memComp);
int llegirDadesClient(int fd, Sincronitzacio sincron, semaphore semFills, infoLloyd * memComp);


//Es crida desde aquí dins
void dadesMeteorologiquesSerializer(char *serial, char *dades);

//Es crida desde parseigDadesDanny
void enviarALloyd(txtFile dadesDanny, char * origen, Sincronitzacio sincron, semaphore semFills, infoLloyd * memComp);


//Danny enviant a Jack
//Es crida desde fileParser
int enviarDadesClient(int socketFD, txtFile txtFile, configDanny *config);

int iniciarServidor(char *ip, int port);
int iniciarclient(char *ip, int port);
int protocolDesconnexio(int fd, char * nom);
int protocolconnexioClient(int fd, char * nom);
char * protocolconnexioServidor(int fd);
int acceptarConnexio(int generalSocketFD);

//WENDY
InfoImatge parseigTramaInicialWendy(char * dades, InfoImatge info);
InfoImatge llegirTramaInicial(int fd);
int gestionarClientWendy(int socketTemp);
int tramaInicialWendy(int fd, char * nom, int mida, char * md5sum);
int enviaBytesImatge(int fd, char * dades);
char * repBytesImatge(int fd, int mida);
int enviaSuccess(int fd);
int enviaError(int fd);
int comprovaMD5(InfoImatge info);
#endif
