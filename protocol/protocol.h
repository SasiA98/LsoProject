#ifndef PROTOCOL_H
#define PROTOCOL_H


#include <stdbool.h>
#include <string.h>

#define DIM_BUFFER 500

#define DIM_PARAMETERS 550

typedef struct parameters{
    int error;	
	int choice;
    int from;
    int to;
    int dimFile;
    char buffer[DIM_BUFFER];
}parameters;

unsigned char * serialize_int(unsigned char *buffer, int value);
unsigned char * serialize_char(unsigned char *buffer, char value);
int deserialize_int(unsigned char *buffer);
char deserialize_char(unsigned char *buffer);
void serializeParameters(unsigned char* msg, parameters *par);
void deserializeParameters(unsigned char* buffer, parameters *par);


#endif