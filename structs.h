#ifndef STRUCTS_H_
#define STRUCTS_H_

#include "semaphore_v2.h"

typedef struct{
  char nom[30];
  int mida[35];
  char md5[32];
} InfoImatge;

typedef struct {
    char nomEstacio[101];
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


typedef struct{
    semaphore semJack;
    semaphore semJack2;
} Sincronitzacio;

#endif
