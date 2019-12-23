#ifndef SERVER_LIBRARY_H
#define SERVER_LIBRARY_H

#include <unistd.h>
#include <stdio.h> 
#include <netdb.h> 
#include <pthread.h>
#include <netinet/in.h> 
#include <stdlib.h>
#include <signal.h>
#include <sys/socket.h> 
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h> 
#include "../protocol/protocol.h"

#define PORT 8080



struct mutex {

} mutex;


typedef struct management{

    int connectfd;
    parameters *par;
}management;


int CreateSocket();
void* mainThread(void* arg);
void* readThread(void* arg);
void* writeThread(void* arg);
void* dimThread(void* arg);


#endif