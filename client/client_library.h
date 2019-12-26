#ifndef CLIENT_LIBRARY_H
#define CLIENT_LIBRARY_H

#include <unistd.h>
#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <sys/socket.h> 
#include <arpa/inet.h>
#include "../protocol/protocol.h"

int PORT;
char IP_PORT[20];

typedef unsigned int uint;

int checkArgsInvalidClient(int argc, const char *argv[]);
void getStr(char * str, uint len);
int getInt();
void clientFunctions(int sockfd);
void dimension(int socketfd, parameters *par);
void readFile(int socketfd ,parameters *par);
void writeFile(int socketfd ,parameters *par);


#endif