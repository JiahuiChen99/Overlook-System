#include <pthread.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include "../fileParser.h"


#define START  "Starting Danny...\n"

int main(int argc, char *argv[]) {
  configJack config;
  int generalSocketFD;
  if(argc < 2){
      char buff[100];
      int bytes = sprintf(buff, ARGUMENT_ERROR);
      write(1, buff, bytes);
      exit(ERROR_RETURN);
  }

  write(1, START, sizeof(START));


  //Primer llegim la configuració
  config.nom = NULL;
  config.ipJack = NULL;
  config.portJack = 0;
  config = llegirConfig(argv[1], "Jack");

  //Iniciem el servidor
  generalSocketFD = iniciarServidor(config.ipJack, config.portJack);

  //Esperem a rebre
  while(1){
    //Fer un accept
    struct sockaddr_in cli_addr;
    socklen_t length = sizeof (s_addr);

    int newsock = accept (sockfd, (void *) &cli_addr, &length);
    if (newsock < 0){
      bytes = sprintf(buff, ACCEPT_ERROR);
      write(1, buff, bytes);
      return -1;
    }
    //Fork per tractar el socket

  }
  return 0;
}
