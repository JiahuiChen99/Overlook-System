#ifndef OVERLOOK_SYSTEM_IMATGEUTILS_H
#define OVERLOOK_SYSTEM_IMATGEUTILS_H

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

char * llegirImatge(int fd);
char * getMD5(char * nomArxiu, char * out);

#endif
