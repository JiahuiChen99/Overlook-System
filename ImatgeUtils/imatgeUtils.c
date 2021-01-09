#include "imatgeUtils.h"

MidaImatge llegirImatge(int fd){
    MidaImatge info;
    info.imatge = (char *) malloc(sizeof(char)*1);
    char buff[100];
    int lletres = 0;
    int comprovacio;
    int counterBytes = 0;
    while (1){
        memset(buff, '\0', 100);
        comprovacio = read(fd, &buff, 100);
        lletres+=comprovacio;
        if((comprovacio < 100)){
            info.imatge = (char *) realloc(info.imatge, sizeof(char)*(lletres));

            for(int i = 0; i < comprovacio; i++){
                info.imatge[counterBytes] = buff[i];
                counterBytes++;
            }

            info.mida = lletres;
            return(info);
        }else{
            info.imatge = (char *) realloc(info.imatge, sizeof(char)*lletres);

            for(int i = 0; i < comprovacio; i++){
                info.imatge[counterBytes] = buff[i];
                counterBytes++;
            }
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
