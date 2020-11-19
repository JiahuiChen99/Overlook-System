#ifndef OVERLOOK_SYSTEM_SOCKETS_H
#define OVERLOOK_SYSTEM_SOCKETS_H
#define _DEFAULT_SOURCE

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define IP_ERROR "%s no es una adreça IP vàlida\n"
#define BIND_ERROR "Error en fer el Bind!\n"
#define LISTEN_ERROR "Error en fer el Listen!\n"
#define ACCEPT_ERROR "Error en fer el Accept!\n"
/*
*Iniciar el servidor amb la ip i port donats
*
*Returns: File Descriptor del socket
*/
int iniciarServidor(char *ip, int port);
int iniciarclient(char *ip, int port);
