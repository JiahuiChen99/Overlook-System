#include "fileParser.h"

int fileDetection(configDanny *config){
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

            closedir(directori);

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
    closedir(directori);
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
    config.nom = llegirCadena(fitxer);

    //Llegim la carpeta on son els arxius
    config.carpeta =  llegirCadena(fitxer);
    //Llegim el temps
    char * aux;

    aux = llegirCadena(fitxer);
    config.temps = atoi(aux);
    //free(aux);

    //Llegim les dades de Jack
    config.ipJack =  llegirCadena(fitxer);

    aux = llegirCadena(fitxer);
    config.portJack= atoi(aux);
    //free(aux);

    //Llegim les dades de Wendy
    config.ipWendy =  llegirCadena(fitxer);

    aux = llegirCadena(fitxer);
    config.portWendy = atoi(aux);
    free(aux);

    //Tanquem el File Descriptor
    close(fitxer);


    return(config);
}
