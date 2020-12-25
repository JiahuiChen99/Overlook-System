#include "imatgeUtils.h"

char * llegirImatge(int fd){
  char *cadena = (char *) malloc(sizeof(char)*1);
  char buff[100];
  int lletres = 0;
  int comprovacio;
  while (1){
      comprovacio = read(fd, &buff, 100);
      lletres+=100;
      if((comprovacio < 100)){
          lletres++;
          cadena = (char *) realloc(cadena, sizeof(char)*(lletres));
          strcat(cadena, buff);
          cadena[lletres-1] = '\0';
          return(cadena);
      }else{
          lletres++;
          cadena = (char *) realloc(cadena, sizeof(char)*lletres);
          strcat(cadena, buff);
      }
  }
}

char * getMD5(char * nomArxiu, char * out){
  char buff[100];
  int fd[2];

  pipe(fd);
  int forkId = fork();
  // En funcio del pid serem pare o fill
  switch (forkId){
     case -1:// Si hi ha error el podem tractar
                break;
     case 0: //ESTEM AL FILL
     // Tanquem el file descriptor de lectura
       close(fd[0]);
       // Escrivim pel file descriptor d'escriptura
      //Reescrivim el filedescriptor de la pantalla per el del pare (pipe)
      dup2(fd[1],1);
      close(fd[1]);
      char *argVec[3];
      argVec[0]="/bin/md5sum";
      argVec[1]=nomArxiu;
      argVec[2]=NULL;
      execv("/bin/md5sum", argVec);
    break;

     default://ESTEM AL PARE
      memset(buff, '\0',100);
      memset(out, '\0',100);

      // Esperem a que el fill mori/acabi
      wait(NULL);
      // Tanquem el file descriptor d'escriptura
      close(fd[1]);
       // Llegim del pipe
       read(fd[0],buff,100);
       // Guardem només el MD5
       int sortir = 0;
       for(int i = 0; !sortir ;i++){
         if (buff[i] != ' ') {
            out[i] = buff[i];
         }else{
           sortir = 1;
         }
       }
       break;
 }
 close(fd[0]);
 close(fd[1]);
 return out;
}
