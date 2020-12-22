#ifndef LLOYD_H
#define LLOYD_H

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
#include <sys/shm.h>

//Llibreries internes
#include "../fileParser.h"
#include "../semaphore_v2.h"
#include "../structs.h"

int processaLloyd();
int crearMemoriaCompartida();
int crearSemafors();

#endif
