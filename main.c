#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>

//Strings necessaris.
#define FILE_ERROR "ERROR. ARXIU DE CONFIGURACIÓ DE %s NO TROBAT\n"
#define ARGUMENT_ERROR "ERROR. NO S'HAN TROBAT SUFICIENTS ARGUMENTS DE PROGRAMA.\n"
#define ERROR_RETURN -1

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

//Variables globals
configDanny config;




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
  char *cadena = (char *) malloc(sizeof(char)*1);
  char buff;
  int lletres = 0;
  int comprovacio;
  while (1){
    comprovacio = read(fd, &buff, sizeof(char));
    if((buff == '\n')||(comprovacio <= 0)){
        return(cadena);
    }else{
      lletres++;
      cadena = (char *) realloc(cadena, sizeof(char)*lletres);
      cadena[lletres-1] = buff;
    }
  }
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
  int fitxer = open(nomFitxer, O_RDONLY);
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
  config.temps = atoi(LlegirCadena(fitxer));

  //Llegim les dades de Jack
  config.ipJack =  LlegirCadena(fitxer);
  config.portJack= atoi(LlegirCadena(fitxer));

  //Llegim les dades de Wendy
  config.ipWendy =  LlegirCadena(fitxer);
  config.portWendy = atoi(LlegirCadena(fitxer));

  //Tanquem el File Descriptor
  close(fitxer);

  return(config);
}

int main(int argc, char *argv[]) {
    if(argc < 2){
      char buff[100];
      int bytes = sprintf(buff, ARGUMENT_ERROR);
      write(1, buff, bytes);
      exit(ERROR_RETURN);
    }
    config = llegirConfig(argv[1]);
    printf("NOM: %s\n", config.nom);
    printf("CARPETA: %s\n", config.carpeta);
    printf("TEMPS: %d\n", config.temps);
    printf("ipJack: %s\n", config.ipJack);
    printf("port Jack: %d\n", config.portJack);
    printf("ipWendy: %s\n", config.ipWendy);
    printf("port Wendy: %d\n", config.portWendy);

    signal(SIGALRM, signalhandler);

    free (config.nom);
    free (config.carpeta);
    free (config.ipJack);
    free (config.ipWendy);
    return 0;
}
