#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

//Strings necessaris.
#define FILE_ERROR "ERROR. ARXIU DE CONFIGURACIÓ DE %s NO TROBAT"
#define ARGUMENT_ERROR "ERROR. NO S'HAN TROBAT SUFICIENTS ARGUMENTS DE PROGRAMA."
#define ERROR_RETURN -1

//Variables globals
configDanny config;

//Estructura que guardarà la informació de configuració de Danny
typedef struct ConfigDanny{
  char * nom;
  char * carpeta;
  int temps;
  char * ipJack;
  int portJack;
  char * ipWendy;
  int portWendy;
}


void signalhandler(int sigint){
  if(sigint == SIGALRM){
    //Comprovar la carpeta i mirar si tenim arxius de text
  }
}

/*
*Llegirem cadenas dels arxius caràcter a caràcter
*
*Args:
* ·fd: File descriptor d'on volem llegir
*
*Retorna: La cadena llegida
*/
char * LlegirCadena(int fd){
  char *cadena;
  char buff;
  int lletres = 0;

  read(fd, buff, sizeof(char));
  if(buff == '\n'){
      return(cadena)
  }else{
    lletres++;
    cadena = (char *) realloc(cadena, sizeof(char)*lletres)
    cadena[lletres-1] = buff;
  }

  return(cadena)
}


/*
*Llegirem i guardarem la informació de l'arxiu de configuració de Danny
*
*Args:
* ·nomFitxer: nom del fitxer d'on volem llegir
*
*Retorna: Struct amb la informació llegida.
*/
configDanny llegirConfig(char *nomFitxer){
  int fitxer=open(nomFitxer, O_RDONLY);
  configDanny config;

  //Comprovem que el fitxer existeixi
  if(fitxer < 0){
    char buff[100];
    int bytes = sprintf(buff, FILE_ERROR, "Danny");
    write(1, buff, bytes);
    exit(ERROR_RETURN);
  }

  //Llegim el nom de la estacio
  config.nom = LlegirCadena(fitxer);
  //Llegim la carpeta on son els arxius
  config.carpeta =  LlegirCadena(fitxer);
  //Llegim el temps
  read(fitxer,&(config).temps,sizeof(int));
  //Llegim les dades de Jack
  config.ipJack =  LlegirCadena(fitxer);
  read(fitxer,&(config).portJack,sizeof(int));
  //Llegim les dades de Wendy
  config.ipWendy =  LlegirCadena(fitxer);
  read(fitxer,&(config).portWendy,sizeof(int));

  //Tanquem el File Descriptor
  close(fitxer);

  return(config);
}

int main(int argc, char const *argv[]) {
    if(argc < 2){
      char buff[100];
      int bytes = sprintf(buff, ARGUMENT_ERROR);
      write(1, buff, bytes);
      exit(ERROR_RETURN);
    }
    config = llegirConfig(argv[1])
    signal(SIGALRM, signalhandler);


    return 0;
}
