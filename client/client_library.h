#ifndef CLIENT_LIBRARY_H
#define CLIENT_LIBRARY_H

#include <unistd.h>
#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <stdbool.h>

#define PORT 8080
#define IP_PORT "127.0.0.1"

typedef unsigned int uint;


int getInt();
void clientFunctions(int sockfd);
int dimension(int socketfd);
void getStr(char * str, uint len);


typedef struct parameters {
	int choice;
    int from;
    int to;
    int dimFile;
    char *buffer[];
}parameters;

#endif