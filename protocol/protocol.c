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
	buffer = serialize_int(buffer,par->error);
    buffer = serialize_int(buffer,par->choice);
    buffer = serialize_int(buffer,par->from);   
	buffer = serialize_int(buffer,par->to);
    buffer = serialize_int(buffer,par->dimFile);

	for(int i=0; i<strlen(par->buffer)+1; i++)
        buffer = serialize_char(buffer,par->buffer[i]);  
}



parameters * deserializeParameters(unsigned char* buffer, parameters *par)
{
    par->error = deserialize_int(buffer);
    par->choice = deserialize_int(buffer+4);
    par->from = deserialize_int(buffer+8);
    par->to = deserialize_int(buffer+12);
    par->dimFile = deserialize_int(buffer+16);    
  
    int i=0;
    int j=20;
    
    do
    {
        par->buffer[i] = deserialize_char((buffer+j));
        i++;
        j++;
    }while(par->buffer[i-1]!='\0'); 
    
    j++; 
    
    return par;
}