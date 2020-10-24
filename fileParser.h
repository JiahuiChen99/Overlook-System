#ifndef OVERLOOK_SYSTEM_FILEPARSER_H
#define OVERLOOK_SYSTEM_FILEPARSER_H

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

//Arxiu configuració Danny
#define FILE_ERROR "ERROR. ARXIU DE CONFIGURACIÓ DE %s NO TROBAT\n"
#define ARGUMENT_ERROR "ERROR. NO S'HAN TROBAT SUFICIENTS ARGUMENTS DE PROGRAMA.\n"
#define ERROR_RETURN -1

//Arxius de dades Danny: .txt & .jpg
#define FILES_FOUND "%d files found\n"
#define NO_FILES_FOUND "No files available\n"
#define NO_SUCH_DIRECTORY "No such directory\n"

typedef struct {
    char * data;
    char * hora;
    float temperatura;
    int humitat;
    float pressio_atmosferica;
    float precipitacio;
} txtFile;

//Estructura que guardarà la informació de configuració de Danny
typedef struct configDanny{
    char * nom;
    char * carpeta;
    int temps;
    char * ipJack;
    int portJack;
    char * ipWendy;
    int portWendy;
}configDanny;

int fileDetection(configDanny *config);
char * llegirCadena(int fd);
configDanny llegirConfig(char *nomFitxer);

#endif //OVERLOOK_SYSTEM_FILEPARSER_H