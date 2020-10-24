#include "fileParser.h"

int fileDetection(configDanny *config){
    DIR *directori;
    struct dirent *directoryFile;
    char buff[50], buff2[50];
    int totalFilesMatching = 0;
    txtFile txtFile;

    memset(buff2, '\0', sizeof(buff2));

    int bytes =sprintf(buff2, "$%s:\n", config->nom);
    write(1, buff2, bytes);

    //Obertura d'escriptori
    //TODO: A matagalls no cal afegir ..
    memset(buff, '\0', sizeof(buff));
    buff[0] = '.';
    buff[1] = '.';
    strcat(buff, config->carpeta);

    /*char cwd[100];
    printf("Current Directory: %s\n", getcwd(cwd, sizeof(cwd)));*/

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
            bytes = sprintf(buff2, FILES_FOUND, totalFilesMatching);
            write(1, buff2, bytes);

            directori = opendir(buff);

            while ((directoryFile = readdir(directori)) != NULL){
                //regex
                if(strstr(directoryFile->d_name, ".txt") != NULL ||
                   strstr(directoryFile->d_name, ".jpg") != NULL){
                    write(1, directoryFile->d_name, sizeof(directoryFile->d_name));
                    write(1, "\n", 1);

                    //Parseig fitxer de dades txt
                    if(strstr(directoryFile->d_name, ".txt") != NULL){
                        int fdFitxer = open(directoryFile->d_name, O_RDONLY);

                        //Comprovem que el fitxer existeixi
                        if(fdFitxer < 0){
                            memset(buff2, '\0', sizeof(buff2));
                            //TODO: CHANGE ERROR
                            bytes = sprintf(buff2, FILE_ERROR, "Danny");
                            write(1, buff2, bytes);
                            exit(ERROR_RETURN);
                        }

                        txtFile.data = llegirCadena(fdFitxer);
                        txtFile.hora = llegirCadena(fdFitxer);
                        txtFile.temperatura = (float) atof(llegirCadena(fdFitxer));
                        txtFile.humitat = atoi(llegirCadena(fdFitxer));
                        txtFile.pressio_atmosferica = (float) atof(llegirCadena(fdFitxer));
                        txtFile.precipitacio = (float) atof(llegirCadena(fdFitxer));

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



                        //TODO: Alliberar memòria i eliminar fitxer
                        close(fdFitxer);
                    }else{
                        //TODO: JPG Parsing
                    }

                }
            }
        }
    }
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
    config.temps = atoi(llegirCadena(fitxer));

    //Llegim les dades de Jack
    config.ipJack =  llegirCadena(fitxer);
    config.portJack= atoi(llegirCadena(fitxer));

    //Llegim les dades de Wendy
    config.ipWendy =  llegirCadena(fitxer);
    config.portWendy = atoi(llegirCadena(fitxer));

    //Tanquem el File Descriptor
    close(fitxer);

    return(config);
}