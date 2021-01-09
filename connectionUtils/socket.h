#ifndef OVERLOOK_SYSTEM_SOCKETS_H
#define OVERLOOK_SYSTEM_SOCKETS_H
#define _XOPEN_SOURCE_EXTENDED

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "../ImatgeUtils/imatgeUtils.h"
#include "../semaphore_v2.h"
#include "../structs.h"

#define IP_ERROR "%s no es una adreça IP vàlida\n"
#define BIND_ERROR "Error en fer el Bind!\n"
#define LISTEN_ERROR "Error en fer el Listen!\n"
#define ACCEPT_ERROR "Error en fer el Accept!\n"

#define ERROR_DE_TRAMA "ERROR EN EL FORMAT DE LA TRAMA!\n"
#define ERROR_DE_DADES "ERROR EN EL FORMAT DE LES DADES!\n"

//Jack - Parseig de dades Danny
#define RECEIVE_DATA "Receiving data...\n"
#define ERROR_DADES_DANNY -1


//Sockets utils
//Cridar desde mainJack.c

/*
*Gestionem la informació que arriba desde el client a Jack
*
*Params:
*   -fd: File Descriptor del socker a llegit
*   -sincron: Estructura per a escalabilitat de semàfors
*   -semFills: semàfor dels fills
*   -memComp: Punter al espai de memòria compartida
*
*Returns: int. Estat de sortida.
*/
int gestionarClient(int fd, Sincronitzacio sincron, semaphore semFills, infoLloyd * memComp);

//Jack rebent Danny
//Es crida desde gestionarClient

/*
*Parseig i comprovació de dades rebudes a Jack
*
*Params:
*   -dadesMeteorologiques: Estructura amb la informació rebuda
*   -sincron: Estructura per a escalabilitat de semàfors
*   -semFills: semàfor dels fills
*   -memComp: Punter al espai de memòria compartida
*
*Returns: int. Estat de sortida. Si les dades son o no correctes
*/
int parseigDadesDanny(osPacket dadesMeteorologiques, Sincronitzacio sincron, semaphore semFills, infoLloyd * memComp);

/*
*Funció per a llegir del client Danny
*
*Params:
*   -fd: socket d'on llegir
*   -sincron: Estructura per a escalabilitat de semàfors
*   -semFills: semàfor dels fills
*   -memComp: Punter al espai de memòria compartida
*
*Returns: int. Estat de sortida.
*/
int llegirDadesClient(int fd, Sincronitzacio sincron, semaphore semFills, infoLloyd * memComp);


//Es crida desde aquí dins
/*
*Funció per a serialitzar les dades Meteorologiques
*
*Params:
*   -serial: string on es guardaràn les dades
*   -dades: String de dades a serialitzar
*
*Returns: --
*/
void dadesMeteorologiquesSerializer(char *serial, char *dades);

//Es crida desde parseigDadesDanny
/*
*Funció per a enviar a Lloyd les dades Meteorologiques
*
*Params:
*   -dadesDanny: Dades a enviar a Lloyd
*   -origen: Nom del origen de les dades
*   -sincron: Estructura per a escalabilitat de semàfors
*   -semFills: semàfor dels fills
*   -memComp: Punter al espai de memòria compartida
*
*Returns: --
*/
void enviarALloyd(txtFile dadesDanny, char * origen, Sincronitzacio sincron, semaphore semFills, infoLloyd * memComp);


//Danny enviant a Jack
//Es crida desde fileParser

/*
*Funció per a enviar a Jack les dades Meteorologiques
*
*Params:
*   -socketFD: FD del socket per on enviarem les dades
*   -txtFile: Informació a enviar llegida del fitxer
*   -config: Fitxer de configuració de Danny.
*
*Returns: int. Estat de sortida.
*/
int enviarDadesClient(int socketFD, txtFile txtFile, configDanny *config);

/*
*Funció per a iniciar el servidor Jack O Wendy
*
*Params:
*   -ip: ip del servidor
*   -port: Port del servidor
*
*Returns: int. Estat de sortida.
*/
int iniciarServidor(char *ip, int port);

/*
*Funció per a iniciar el client cap a el servidor Jack O Wendy
*
*Params:
*   -ip: ip del servidor
*   -port: Port del servidor
*
*Returns: int. Estat de sortida.
*/
int iniciarclient(char *ip, int port);

/*
*Funció per al protocol de desconnexió
*
*Params:
*   -fd: Socket per on escriure
*   -nom: Nom de la estació
*
*Returns: int. Estat de sortida.
*/
int protocolDesconnexio(int fd, char * nom);

/*
*Funció per al protocol de connexió del client
*
*Params:
*   -fd: Socket per on escriure
*   -nom: Nom de la estació
*
*Returns: int. Estat de sortida.
*/
int protocolconnexioClient(int fd, char * nom);

/*
*Funció per al protocol de connexió per part del servidor
*
*Params:
*   -fd: Socket per on escriure
*   -nom: Nom de la estació
*
*Returns: int. Estat de sortida.
*/
char * protocolconnexioServidor(int fd);

/*
*Funció per al acceptar connexions des de'l servidor
*
*Params:
*   -generalSocketFD: Socket per on rebre la connexió
*
*Returns: int. Estat de sortida.
*/
int acceptarConnexio(int generalSocketFD);

//WENDY

/*
*Funció de parseig de les dades de trama inicial en rebre-la
*
*Params:
*   -dades: dades a llegir
*   -info: estructura on es guardaràn les dades
*
*Returns: Infoimatge. Estructura de informació
*/
InfoImatge parseigTramaInicialWendy(char * dades, InfoImatge info);

/*
*Funció de lectura de trama inicial en rebre-la
*
*Params:
*   -fd: socket d'on es llegirà
*
*Returns: Infoimatge. Estructura de informació
*/
InfoImatge llegirTramaInicial(int fd);

/*
*Funció on es gestionarà allò relacionat amb un client de Wendy. Es l'espai de execució d'un fill de Wendy
*
*Params:
*   -socketTemp: socket d'on es llegirà
*
*Returns: Infoimatge. Estructura de informació
*/
int gestionarClientWendy(int socketTemp);

/*
*Funció d'enviament de trama inicial cap a Wendy
*
*Params:
*   -fd: socket per on s'enviarà
*   -nom: nom de la imatge
*   -mida: mida de la imatge
*   -md5sum: md5 de la imatge
*
*Returns: int. Estat de sortida.
*/
int tramaInicialWendy(int fd, char * nom, int mida, char * md5sum);

/*
*Funció d'enviament de bytes de la imatge cap a Wendy
*
*Params:
*   -fd: socket per on s'enviarà
*   -dades: bytes de la imatge
*
*Returns: int. Estat de sortida.
*/
int enviaBytesImatge(int fd, char * dades);

/*
*Funció de recepció de bytes de la imatge a Wendy
*
*Params:
*   -fd: socket per on es rebrà la informació
*   -mida: mida de la imatge
*
*Returns: char *. La imatge.
*/
char * repBytesImatge(int fd, int mida);

/*
*Funció d'enviament de la trama de resposta d'exit desde Wendy cap a Danny.
*
*Params:
*   -fd: socket per on s'envia la informació
*
*Returns: int. Estat de sortida.
*/
int enviaSuccess(int fd);

/*
*Funció d'enviament de la trama de resposta de error desde Wendy cap a Danny.
*
*Params:
*   -fd: socket per on s'envia la informació
*
*Returns: int. Estat de sortida.
*/
int enviaError(int fd);

/*
*Funció de comrpovament de l'MD5 de la imatge
*
*Params:
*   -info: informació de la imatge.
*
*Returns: int. 0 si es correcte, -1 si es incorrecte.
*/
int comprovaMD5(InfoImatge info);
#endif
