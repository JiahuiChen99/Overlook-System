#include "socket.h"

int iniciarServidor(char *ip, int port){
  char buff[100];
  int bytes;
  //Creem el socket
  int socketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  //Preparem la estructura
  struct sockaddr_in s_addr;
  memset (&s_addr, 0, sizeof (s_addr));
  s_addr.sin_family = AF_INET;
  s_addr.sin_port = htons (port);

  if (inet_aton (ip, &s_addr.sin_addr) == 0){
    bytes = sprintf(buff, IP_ERROR, ip);
    write(1, buff, bytes);
    return -1;
  }
  //Executem el bind per indicar que esperem rebre info
  if(bind(socketFD, &s_addr, sizeof(s_addr)) < 0){
    bytes = sprintf(buff, BIND_ERROR);
    write(1, buff, bytes);
    return -1;
  }

  if(listen(socketFD, 3)){
    bytes = sprintf(buff, LISTEN_ERROR);
    write(1, buff, bytes);
    return -1;
  }

}

int iniciarclient(char *ip, int port){
  char buff[100];
  int bytes
  //Creem el socket
  int socketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  struct sockaddr_in s_addr;
  memset (&s_addr, 0, sizeof (s_addr));
  s_addr.sin_family = AF_INET;
  s_addr.sin_port = htons (port);

  if (inet_aton (ip, &s_addr.sin_addr) == 0){
    bytes = sprintf(buff, IP_ERROR, ip);
    write(1, buff, bytes);
    return -1;
  }
  //Executem el bind per indicar que esperem rebre info
  if(connect(socketFD, (void *) &s_addr, sizeof(s_addr)) < 0){
    bytes = sprintf(buff, BIND_ERROR);
    write(1, buff, bytes);
    return -1;
  }

  return socketFD;
}

int gestionarClient(int fd){
  int finish;
  do{
    finish = llegirDadesClient(fd);
  }while(finish < 0);
  return 0;
}
