#include "protocol.h"

unsigned char * serialize_int(unsigned char *buffer, int value)
{
    buffer[0] = value >> 24;
    buffer[1] = value >> 16;
    buffer[2] = value >> 8;
    buffer[3] = value;
    return buffer + 4;
}

unsigned char * serialize_char(unsigned char *buffer, char value)
{
    buffer[0] = value;
    return buffer + 1;
}

int deserialize_int(unsigned char *buffer)
{
    int value = 0;

    value |= buffer[0] << 24;
    value |= buffer[1] << 16;
    value |= buffer[2] << 8;
    value |= buffer[3];
    return value;

}

char deserialize_char(unsigned char *buffer)
{
    return buffer[0];
}


void serializeParameters(unsigned char* buffer, parameters *par)
{
    parameters *temp = par;
	buffer = serialize_int(buffer,temp->numRequest);
	buffer = serialize_int(buffer,temp->error);
    buffer = serialize_int(buffer,temp->choice);
    buffer = serialize_int(buffer,temp->from);   
	buffer = serialize_int(buffer,temp->to);
    buffer = serialize_int(buffer,temp->dimFile);

	for(int i=0; i<strlen(temp->buffer)+1; i++)
        buffer = serialize_char(buffer,temp->buffer[i]);  
}


void deserializeParameters(unsigned char* buffer, parameters *par)
{
    parameters *temp = par;
    temp->numRequest = deserialize_int(buffer);
    temp->error = deserialize_int(buffer+4);
    temp->choice = deserialize_int(buffer+8);
    temp->from = deserialize_int(buffer+12);
    temp->to = deserialize_int(buffer+16);
    temp->dimFile = deserialize_int(buffer+20);    
  
    int i=0;
    int j=24;
    
    do
    {
        temp->buffer[i] = deserialize_char((buffer+j));
        i++;
        j++;
    }while(temp->buffer[i-1]!='\0'); 
    
    j++; 

}