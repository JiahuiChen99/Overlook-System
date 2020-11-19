#include <pthread.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include "../fileParser.h"


#define START  "Starting Jack...\n"

int main(int argc, char *argv[]) {
  configJack config;
  int generalSocketFD;

  //Fem una ràpida comprovació d'arguments
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
  llegirConfig(argv[1], "Jack", NULL, &config);

  //Iniciem el servidor
  generalSocketFD = iniciarServidor(config.ipJack, config.portJack);

  //Esperem a rebre
  int socketsClients[3];
  int forkIDsClients[3];
  int socketCounter = 0;
  int forkCounter = 0;
  while(1){
    //Fer un accept
    struct sockaddr_in cli_addr;
    socklen_t length = sizeof (s_addr);

    socketsClients[socketCounter] = accept (sockfd, (void *) &cli_addr, &length);
    if (newsock < 0){
      bytes = sprintf(buff, ACCEPT_ERROR);
      write(1, buff, bytes);
      return -1;
    }
    socketCounter = socketCounter == 2 ? 0 : socketCounter+1
    //Fork per tractar el socket
    forkIDsClients[forkCounter] = fork();
    switch (forkIDsClients[forkCounter]) {
      case -1:
        //Display error
      break;
      case 0:
        //fill
        //Començar a mirar qué ens envien i printar.
        gestionarClient(socketsClients[socketCounter]);
        return 0;
      break;
      default:
        //Pare
        forkCounter = forkCounter == 2 ? 0 : forkCounter+1
      break;
    }

  }
  return 0;
}
