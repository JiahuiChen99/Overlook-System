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
#include "./ImatgeUtils/imatgeUtils.h"

//Arxiu configuració Danny
#define FILE_ERROR "ERROR. ARXIU DE CONFIGURACIÓ DE %s NO TROBAT\n"
#define ARGUMENT_ERROR "ERROR. NO S'HAN TROBAT SUFICIENTS ARGUMENTS DE PROGRAMA.\n"
#define ERROR_RETURN -1

//Arxius de dades Danny: .txt & .jpg
#define FILES_FOUND "%d files found\n"
#define DATA_FILE_NOT_FOUND "%s file not found\n"
#define NO_FILES_FOUND "No files available\n\n"
#define NO_SUCH_DIRECTORY "No such directory\n"
#define TRAMA_INICIAL_ERROR "Error en enviar la trama inicial a Wendy\n"
#define IMATGE_ERROR "Error en enviar bytes de la imatge a Wendy\n"


/*
*Buscar fitxer's d'una carpeta
*
*Params:
*   -config: estructura amb la informació de ocnfiguració de Danny
*   -socket: Socket que connecta amb Jack
*   -socketW: Socket que connecta amb Wendy
*   -fdFitxer: File Descriptor de fitxer
*
*Returns: Si ha tingut èxit o no
*/
int fileDetection(configDanny *config, int socket, int socketW, int fdFitxer);

/*
*Llegim una línia de un fitxer
*
*Params:
*   -fd: File Descriptor de fitxer a llegir
*
*Returns: la cadena llegida
*/
char * llegirCadena(int fd);

/*
*Llegim la configuració del programa
*
*Params:
*   -nomFitxer: Nom del fitxer de configuració a llegir
*   -process: nom del procés que s'executa
*   -configDanny: estructura a modificar; configuració de Danny
*   -configJack:  estructura a modificar; configuració de Jack
*
*Returns: --
*/
void llegirConfig(char *nomFitxer, char *process, struct configDanny *configDanny, struct configJack *configJack);


#endif //OVERLOOK_SYSTEM_FILEPARSER_H
