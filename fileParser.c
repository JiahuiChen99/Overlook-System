#include "fileParser.h"


int parseigDadesDanny(osPacket dadesMeteorologiques){
    int i, j, hashtagCounter;
    char *aux = NULL;
    aux = (char *) malloc(sizeof(char) * 1);
    i = j = 0;

    while(i < 100){
        while(dadesMeteorologiques.dades[i] != '#'){
            aux[j] = dadesMeteorologiques.dades[i];
            aux = (char *)realloc(aux, sizeof(char)* (j + 2));
            i++;
            j++;
        }

        switch (hashtagCounter) {
            case 0:
                //Comprovar data
                if(strlen(aux) != 10){
                    return ERROR_DADES_DANNY;
                }
                break;
            case 1:
                //Comprovar hora
                if(strlen(aux) != 8){
                    return ERROR_DADES_DANNY;
                }
                break;
            case 2:
                //Comprovar temperatura
                if(strlen(aux) > 5){
                    return ERROR_DADES_DANNY;
                }
                break;
            case 3:
                //Comprovar humitat
                if(strlen(aux) > 3){
                    return ERROR_DADES_DANNY;
                }
                break;
            case 4:
                //Comprovar pressió
                if(strlen(aux) > 6){
                    return ERROR_DADES_DANNY;
                }
                break;
            case 5:
                //Comprovar precipitació
                if(strlen(aux) > 4){
                    return ERROR_DADES_DANNY;
                }
                break;
            default:
                break;
        }
        hashtagCounter++;
        i++;
        j = 0;
    }

    return 0;
}

int llegirDadesClient(int socketFD){
    //TODO: no entra aquí???
    printf("Entro a llegir dades\n");
    char trama[sizeof(osPacket)], serial[115];
    osPacket dadesMeteorologiques, tramaResposta;
    printf("ESPERANT DADA\n");
    int nBytes = read(socketFD, trama, sizeof(osPacket));
    printf("DADA REBUDA\n");
    //Inicialització del serial
    memset(serial, '\0', sizeof(serial));
    //Inicialització de la trama
    memset(tramaResposta.origen, '\0', sizeof(tramaResposta.origen));
    tramaResposta.tipus = '\0';
    memset(tramaResposta.dades, '\0', sizeof(tramaResposta.dades));

    //Control dels bytes rebuts, si es perden bytes durant la comunicació error
    if(nBytes != sizeof(osPacket) || trama[14] != 'D'){

        //Popular la trama amb ERROR Trama
        strcpy(tramaResposta.origen, "JACK");
        tramaResposta.tipus = 'Z';
        strcpy(tramaResposta.dades, "ERROR DE TRAMA");

    }else{

        /** Lectura de trama **/
        //Lectura de l'origen
        strncpy(dadesMeteorologiques.origen, trama, 4);
        //Lectura tipus
        dadesMeteorologiques.tipus = trama[14];
        //Lectura de dades
        strncpy(dadesMeteorologiques.dades, trama + 15, 100);

        int dadesStatus = parseigDadesDanny(dadesMeteorologiques);

        if(dadesStatus == ERROR_DADES_DANNY){
            //Popular la trama amb KO
            strcpy(tramaResposta.origen, "JACK");
            tramaResposta.tipus = 'K';
            strcpy(tramaResposta.dades, "DADES KO");
        }else{
            //Popular la trama amb OK
            strcpy(tramaResposta.origen, "JACK");
            tramaResposta.tipus = 'B';
            strcpy(tramaResposta.dades, "DADES OK");
        }
    }

    //Serialitzar
    strcpy(serial, tramaResposta.origen);
    serial[strlen(serial) + 1] =  tramaResposta.tipus;
    strcat(serial, tramaResposta.dades);

    //Enviar
    write(socketFD, serial, sizeof(serial));


  return 0;
}

int enviarDadesClient(int socketFD, txtFile txtFile, configDanny *config){

  //Inicialització del serial
  char serial[115];
  char aux[10];
  memset(aux, '\0', sizeof(aux));
  memset(serial, '\0', sizeof(serial));
  //Inicialització de la trama
  osPacket message;
  memset(message.origen, '\0', sizeof(message.origen));
  message.tipus = '\0';
  memset(message.dades, '\0', sizeof(message.dades));

  // enviar nom estació
  strcpy(message.origen,config->nom);
  message.tipus = 'D';
  //TODO: enviar tota la info
  int index=0;
  strcpy(message.dades,txtFile.data);
  index += strlen(txtFile.data);
  message.dades[index]='#';

  strcat(message.dades,txtFile.hora);
  index += strlen(txtFile.data);
  message.dades[index]='#';

  sprintf(aux, "%f%c", txtFile.temperatura, '\0');
  strcat(message.dades,aux);
  index += strlen(aux);
  message.dades[index]='#';

  sprintf(aux, "%d%c", txtFile.humitat, '\0');
  strcat(message.dades,aux);
  index += strlen(aux);
  message.dades[index]='#';

  sprintf(aux, "%f%c", txtFile.pressio_atmosferica, '\0');
  strcat(message.dades,aux);
  index += strlen(aux);
  message.dades[index]='#';

  sprintf(aux, "%f%c", txtFile.precipitacio, '\0');
  strcat(message.dades,aux);
  index += strlen(aux);
  message.dades[index]='\0';


  //Serialitzar
  //strcpy(serial, message.origen);
  memcpy(serial, message.origen, sizeof(message.origen));
  serial[strlen(serial) + 1] =  message.tipus;
  strncat(serial, message.dades, sizeof(message.dades));


  write(socketFD, serial, 115);

  //Esperem la resposta i la examinem
  read(socketFD, serial, 115);
  osPacket resposta;

  //Inicialitzem
  memset(resposta.origen, '\0', sizeof(resposta.origen));
  resposta.tipus = '\0';
  memset(resposta.dades, '\0', sizeof(resposta.dades));
  //Deserialitzem
  /** Lectura de trama **/
  //Lectura de l'origen
  strncpy(resposta.origen, serial, 4);
  //Lectura tipus
  resposta.tipus = serial[14];
  //Lectura de dades
  strncpy(resposta.dades, serial + 15, 100);
  switch(resposta.tipus){
    case 'Z':
      //Error de trames
      write(1, ERROR_DE_TRAMA, strlen(ERROR_DE_TRAMA));
    break;
    case 'B':
      //Error de dades
      write(1, ERROR_DE_DADES, strlen(ERROR_DE_DADES));
    break;
    default:
      //Tot Correcte
    break;
  }


  return 0;
}

int fileDetection(configDanny *config, int socket){
    DIR *directori;
    struct dirent *directoryFile;
    char buff2[500];
    char * buff = NULL;
    int totalFilesMatching = 0;
    txtFile txtFile;
    memset(buff2, '\0', sizeof(buff2));

    int bytes =sprintf(buff2, "$%s:\n", config->nom);
    write(1, "\n", 1);
    write(1, buff2, bytes);


    //Obertura d'escriptori
    /**
    * buff guarda la direcció de la carpeta de dades que ha de llegir Danny
    * Aquest bloc de sota afegim . + /carpeta + / + '\0'
    **/
    buff = (char *) malloc(sizeof(char)* (1 + (int)strlen(config->carpeta) + 2));

    memset(buff, '\0', 1 + (int)strlen(config->carpeta) + 2);
    buff[0] = '.';
    strcat(buff, config->carpeta);
    buff[strlen(buff)] = '/';
    buff[strlen(buff)] = '\0';


    write(1, "Testing...\n", sizeof("Testing...\n"));

    directori = opendir(buff);


    if( directori == NULL){
        write(1, NO_SUCH_DIRECTORY, sizeof(NO_SUCH_DIRECTORY));
        return -1;
    } else {
        if(readdir(directori) == NULL){
            write(1, NO_FILES_FOUND, sizeof(NO_FILES_FOUND));
            return 0;
        }else{

            while ((directoryFile = readdir(directori)) != NULL){
                //regex
                if(strstr(directoryFile->d_name, ".txt") != NULL ||
                   strstr(directoryFile->d_name, ".jpg") != NULL){
                    totalFilesMatching++;
                }
            }
            if(totalFilesMatching == 0){
                write(1, NO_FILES_FOUND, sizeof(NO_FILES_FOUND));
                return 0;
            }
            bytes = sprintf(buff2, FILES_FOUND, totalFilesMatching);
            write(1, buff2, bytes);

            directori = opendir(buff);

            //Mostrar el nom de tots els arxius
            char *dirname = (char *) malloc(sizeof(char)* ( strlen(buff) + 1));
            strcpy(dirname, buff);
            struct dirent **arxius;
            int q_arxius = scandir(dirname, &arxius, NULL, NULL);
            for (int i = 0; i < q_arxius; i++) {
                if(strstr(arxius[i]->d_name, ".txt") != NULL ||
                   strstr(arxius[i]->d_name, ".jpg") != NULL){
                    write(1, arxius[i]->d_name, strlen(arxius[i]->d_name));
                    write(1, "\n", 1);
                }
                free(arxius[i]);
            }
            free(arxius);
            free(dirname);

            while ((directoryFile = readdir(directori)) != NULL){
                //regex
                if(strstr(directoryFile->d_name, ".txt") != NULL ||
                   strstr(directoryFile->d_name, ".jpg") != NULL){
                    write(1, "\n", 1);
                    write(1, directoryFile->d_name, strlen(directoryFile->d_name));
                    write(1, "\n", 1);

                    //Parseig fitxer de dades txt
                    if(strstr(directoryFile->d_name, ".txt") != NULL){
                        char * fitxerActual = (char *) malloc(sizeof(char)*((strlen(buff)+strlen(directoryFile->d_name) + 1)));
                        strcpy(fitxerActual, buff);
                        strcat(fitxerActual,directoryFile->d_name);

                        int fdFitxer = open(fitxerActual, O_RDONLY);

                        //Comprovem que el fitxer existeixi
                        if(fdFitxer < 0){
                            memset(buff2, '\0', sizeof(buff2));
                            bytes = sprintf(buff2, DATA_FILE_NOT_FOUND, directoryFile->d_name);
                            write(1, buff2, bytes);
                            exit(ERROR_RETURN);
                        }


                        char * aux;
                        txtFile.data = llegirCadena(fdFitxer);
                        txtFile.hora = llegirCadena(fdFitxer);

                        aux = llegirCadena(fdFitxer);
                        txtFile.temperatura = (float) atof(aux);
                        free(aux);

                        aux = llegirCadena(fdFitxer);
                        txtFile.humitat = atoi(aux);
                        free(aux);

                        aux = llegirCadena(fdFitxer);
                        txtFile.pressio_atmosferica = (float) atof(aux);
                        free(aux);

                        aux = llegirCadena(fdFitxer);
                        txtFile.precipitacio = (float) atof(aux);
                        free(aux);

                        write(1, txtFile.data, strlen(txtFile.data));
                        write(1, "\n", 1);
                        write(1, txtFile.hora, strlen(txtFile.hora));
                        write(1, "\n", 1);
                        bytes = sprintf(buff2, "%.1f", txtFile.temperatura);
                        write(1, buff2, bytes);
                        write(1, "\n", 1);
                        bytes = sprintf(buff2, "%d", txtFile.humitat);
                        write(1, buff2, bytes);
                        write(1, "\n", 1);
                        bytes = sprintf(buff2, "%.1f", txtFile.pressio_atmosferica);
                        write(1, buff2, bytes);
                        write(1, "\n", 1);
                        bytes = sprintf(buff2, "%.1f", txtFile.precipitacio);
                        write(1, buff2, bytes);
                        write(1, "\n", 1);


                        enviarDadesClient(socket, txtFile, config);

                        //Alliberar memòria i eliminar fitxer
                        close(fdFitxer);

                        remove(fitxerActual);

                        free(txtFile.data);
                        free(txtFile.hora);
                    }else{
                        //TODO: JPG Parsing
                    }

                }
            }
        }
    }
    free(directoryFile);
    free(directori);
    free(buff);
    return 0;
}


/*
*Llegirem cadenas dels arxius caràcter a caràcter
*
*Args:
* ·fd: File descriptor d'on volem llegir
*
*Retorna: La cadena llegida
*/
char * llegirCadena(int fd){
    char *cadena = (char *) malloc(sizeof(char)*1);
    char buff;
    int lletres = 0;
    int comprovacio;
    while (1){
        comprovacio = read(fd, &buff, sizeof(char));
        if((buff == '\n')||(comprovacio <= 0)){
            cadena = (char *) realloc(cadena, sizeof(char)*lletres);
            cadena[lletres-1] = '\0';
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
void llegirConfig(char *nomFitxer, char *process, struct configDanny *configDanny, struct configJack *configJack){
    int fitxer = open(nomFitxer, O_RDONLY);
    char * aux;

    //Comprovem que el fitxer existeixi
    if(fitxer < 0){
        char buff[100];
        int bytes = sprintf(buff, FILE_ERROR, "Danny");
        write(1, buff, bytes);
        exit(ERROR_RETURN);
    }


    if(strcmp("Danny", process)==0){
        //Llegim el nom de la estació
        configDanny->nom = llegirCadena(fitxer);
        //Llegim la carpeta on son els arxius
        configDanny->carpeta =  llegirCadena(fitxer);
        //Llegim el temps
        aux = llegirCadena(fitxer);
        configDanny->temps = atoi(aux);
    }

    /** Depenent de "a qui" li estiguem llegint la config ho guardem a configDanny o configJack **/
    if(strcmp("Danny", process)==0){
        configDanny->ipJack =  llegirCadena(fitxer);

        aux = llegirCadena(fitxer);
        configDanny->portJack= atoi(aux);
    }else{
        configJack->ipJack =  llegirCadena(fitxer);

        aux = llegirCadena(fitxer);
        configJack->portJack= atoi(aux);
    }



    if(strcmp("Danny", process)==0){
        //Llegim les dades de Wendy
        configDanny->ipWendy =  llegirCadena(fitxer);

        aux = llegirCadena(fitxer);
        configDanny->portWendy = atoi(aux);
    }

    free(aux);

    //Tanquem el File Descriptor
    close(fitxer);

}
