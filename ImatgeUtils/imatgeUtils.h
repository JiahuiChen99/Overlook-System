#ifndef OVERLOOK_SYSTEM_IMATGEUTILS_H
#define OVERLOOK_SYSTEM_IMATGEUTILS_H

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>

#include "../structs.h"

/*
*Llegim una imatge
*
*Params:
*   -fd: File Descriptor de la imatge a llegir
*
*Returns: MidaImatge
*/
MidaImatge llegirImatge(int fd);

/*
*Obtenim el MD5 de una imatge
*
*Params:
*   -fd: File Descriptor de fitxer a llegir
*
*Returns: --
*/
char * getMD5(char * nomArxiu, char * out);

#endif
