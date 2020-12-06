#ifndef STRUCTS_H_
#define STRUCTS_H_

typedef struct {
    char ** nomEstacio;
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


#endif
