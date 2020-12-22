#ifndef OVERLOOK_SYSTEM_FILEPARSER_H
#define OVERLOOK_SYSTEM_FILEPARSER_H
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

#include "./structs.h"
#include "./connectionUtils/socket.h"

//Arxiu configuració Danny
#define FILE_ERROR "ERROR. ARXIU DE CONFIGURACIÓ DE %s NO TROBAT\n"
#define ARGUMENT_ERROR "ERROR. NO S'HAN TROBAT SUFICIENTS ARGUMENTS DE PROGRAMA.\n"
#define ERROR_RETURN -1

//Arxius de dades Danny: .txt & .jpg
#define FILES_FOUND "%d files found\n"
#define DATA_FILE_NOT_FOUND "%s file not found\n"
#define NO_FILES_FOUND "No files available\n\n"
#define NO_SUCH_DIRECTORY "No such directory\n"


char * getMD5(char * nomArxiu, char * out);
char * llegirImatge(int fd);
int fileDetection(configDanny *config, int socket);
char * llegirCadena(int fd);
void llegirConfig(char *nomFitxer, char *process, struct configDanny *configDanny, struct configJack *configJack);


#endif //OVERLOOK_SYSTEM_FILEPARSER_H
