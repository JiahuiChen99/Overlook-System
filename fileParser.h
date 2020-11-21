#ifndef OVERLOOK_SYSTEM_FILEPARSER_H
#define OVERLOOK_SYSTEM_FILEPARSER_H
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
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

//Jack - Parseig de dades Danny
#define RECEIVE_DATA "Receiving data...\n"
#define ERROR_DADES_DANNY -1

typedef struct {
    char origen[14];
    char tipus;
    char dades[100];
} osPacket;

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


//Estructura que guardarà la informació de configuració de Jack
typedef struct configJack{
    char * nom;
    char * ipJack;
    int portJack;
}configJack;

int parseigDadesDanny(osPacket dadesMeteorologiques);
int llegirDadesClient(int fd);
void dadesMeteorologiquesSerializer(char *serial, char *dades);
int enviarDadesClient(int socketFD, txtFile txtFile, configDanny *config);
int fileDetection(configDanny *config, int socket);
char * llegirCadena(int fd);
void llegirConfig(char *nomFitxer, char *process, struct configDanny *configDanny, struct configJack *configJack);

#endif //OVERLOOK_SYSTEM_FILEPARSER_H
