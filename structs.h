#ifndef STRUCTS_H_
#define STRUCTS_H_

#include "semaphore_v2.h"

//Estructura per a ser retornada de la funció de llegir imatge. Conté una imatge i la seva mida
typedef struct{
    int mida;
    char * imatge;
}MidaImatge;

//Estructura de informació relativa a una imatge
typedef struct{
    char nom[30];
    int mida;
    char md5[33];
} InfoImatge;

//Estructura de dades compartida entre Jack i Lloyd
typedef struct {
    char nomEstacio[101];
    float temperatura;
    int humitat;
    float pressio_atmosferica;
    float precipitacio;
} infoLloyd;

//Estructura de dades de un fitxer txt
typedef struct {
    char * data;
    char * hora;
    float temperatura;
    int humitat;
    float pressio_atmosferica;
    float precipitacio;
} txtFile;

//Estructura per a la comunicació
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

//Estructura que guardarà la informació de configuració de Jack i Wendy
typedef struct configJack{
    char * nom;
    char * ipJack;
    int portJack;
}configJack;

//Estructura per a escalabilitat de semàfors
typedef struct{
    semaphore semJack;
    semaphore semJack2;
} Sincronitzacio;

#endif
