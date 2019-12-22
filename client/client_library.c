#include "client_library.h"

unsigned char * serialize_int(unsigned char *buffer, int value);
unsigned char * serialize_char(unsigned char *buffer, char value);
int deserialize_int(unsigned char *buffer);
char deserialize_char(unsigned char *buffer);
unsigned char* serializeParameters(unsigned char* msg, parameters par);
parameters deserializeParameters(unsigned char* buffer, parameters par2);


parameters par;


void getStr(char * str, uint len)
{
  assert(str != NULL);
  uint i;
  char c;
  for(i = 0; (i < len) && ((c = getchar()) != '\n') && (c != EOF); ++i)
    {
      str[i] = c;
    }
  str[i] = '\0';
}

int getInt(){
    int num;
    bool vuoto;
    char* fine;
    char* temp=(char*)malloc(sizeof(char)* 20);
    do {
        getStr(temp, 10);

        if(strlen(temp)==0) vuoto=true;
        else vuoto=false;

        num = strtol(temp, &fine, 10);
        if(*fine!='\0' || vuoto) printf("\nValore errato, inserire di nuovo:");
    }while (*fine!='\0' || vuoto);
    free(temp);
    return  num;
}

void clientFunctions(int socketfd){ 

	while(1) {  
		printf("Inserisci 1 se vuoi conoscere la dimensione del file\nInserisci 2 per leggere da file\nInserisci 3 per scrivere su file\nInserisci 0 per uscire\nInserito: ");
		
		par.choice=getInt();

		while(par.choice<0 || par.choice>3){
			printf("Valore fuori dal range!\nInserire di nuovo:");
			par.choice=getInt();
		}
        par.from=12;
        par.to=14;
        par.dimFile=12;
        strcpy(par.buffer,"Ciao");


        unsigned char buffer[100]; //Controllare i tipi : sasy
	    serializeParameters(buffer, par); 
		write(socketfd,&buffer,sizeof(buffer));

		if(par.choice==0){
			printf("Esco.\n");
			//free(dataParameters);
			break;
		}


		switch (par.choice){
			case 1: dimension(socketfd);
			break;
			case 2: readFile(socketfd);
			break;
			case 3: writeFile(socketfd);
			break;
		}
	}
}

void dimension(int socketfd){

	unsigned char buffer[100];
	read(socketfd, &buffer, sizeof(buffer));
	par = deserializeParameters(buffer, par);

    printf("1: %d\n\n",par.choice); 
    printf("2: %d\n\n",par.from); 
    printf("3: %d\n\n",par.to); 
    printf("4: %s\n\n",par.buffer); 

}

void readFile(int socketfd){
    printf("||Qui si legge dal file.\n");

}

void writeFile(int socketfd){
    printf("Qui si scrive sul file.\n");

}












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


unsigned char* serializeParameters(unsigned char* buffer, parameters par)
{
    buffer = serialize_int(buffer,par.choice);
    buffer = serialize_int(buffer,par.from);   
	buffer = serialize_int(buffer,par.to);
    buffer = serialize_int(buffer,par.dimFile);

	for(int i=0; i<strlen(par.buffer)+1; i++)
        buffer = serialize_char(buffer,par.buffer[i]);  
       
    return buffer;
}


parameters deserializeParameters(unsigned char* buffer, parameters par)
{
    par.choice = deserialize_int(buffer);
    par.from = deserialize_int(buffer+4);
    par.to = deserialize_int(buffer+8);
    par.dimFile = deserialize_int(buffer+12);    
  
    int i=0;
    int j=16;
    
    do
    {
        par.buffer[i] = deserialize_char((buffer+j));
        i++;
        j++;
    }while(par.buffer[i-1]!='\0'); 
    
    j++; 
    
    return par;
}



