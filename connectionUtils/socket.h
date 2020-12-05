#ifndef OVERLOOK_SYSTEM_SOCKETS_H
#define OVERLOOK_SYSTEM_SOCKETS_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "../fileParser.h"
#include "../semaphore_v2.h"

//#include "../tipus.h"
#define IP_ERROR "%s no es una adreça IP vàlida\n"
#define BIND_ERROR "Error en fer el Bind!\n"
#define LISTEN_ERROR "Error en fer el Listen!\n"
#define ACCEPT_ERROR "Error en fer el Accept!\n"

#define ERROR_DE_TRAMA "ERROR EN EL FORMAT DE LA TRAMA!\n"
#define ERROR_DE_DADES "ERROR EN EL FORMAT DE LES DADES!\n"

/* Lo ponemos en fileparser.h porque si no no compila
typedef struct {
    char origen[14];
    char tipus;
    char dades[100];
} osPacket;
*/

/*
*Iniciar el servidor amb la ip i port donats
*
*Returns: File Descriptor del socket
*/
int iniciarServidor(char *ip, int port);
int iniciarclient(char *ip, int port);
int gestionarClient(int fd, char *nomclient, semaphore semJack, semaphore semFills, infoLloyd * memComp);
int protocolDesconnexio(int fd, char * nom);
int protocolconnexioClient(int fd, char * nom);
char * protocolconnexioServidor(int fd);
#endif
