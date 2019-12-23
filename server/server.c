#include <unistd.h>
#include <stdio.h> 
#include <netdb.h> 
#include <pthread.h>
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h>
#include <signal.h>
#include <sys/socket.h> 
#include <sys/types.h> 
#include <stdbool.h>
#include <fcntl.h>
#include <sys/stat.h> 


#define PORT 8080


struct mutex {

} mutex;


typedef struct parameters{
    bool error;	
	int choice;
    int from;
    int to;
    int dimFile;
    char buffer[100];
}parameters;


typedef struct management{

    int connectfd;
    parameters *par;
}management;
 

unsigned char * serialize_int(unsigned char *buffer, int value);
unsigned char * serialize_char(unsigned char *buffer, char value);
int deserialize_int(unsigned char *buffer);
char deserialize_char(unsigned char *buffer);
void serializeParameters(unsigned char* msg, parameters *par);
parameters *deserializeParameters(unsigned char* buffer, parameters *par);


void* readThread(void* arg){   //I thinked to make an infinit cicle to wait the correct input of the client, but if the client dies, the server remains in loop and the management of mutex is more difficult : sasi
	management *man = ((management *) arg);

	int fd, offset;
	char bufferFile[1000];
	unsigned char buffer[1000];

	int bufferSize = man->par->to - man->par->from; // when read from keyboard check that from is smaller then to : sasy 

    if ((fd = open("Document.txt", O_RDONLY)) == -1) //Does it need any permission? : sasy    
	    perror("open error");   
  
    if ((offset = lseek(fd, (off_t) 0, SEEK_END)) == -1)
	     perror("lseek error");      
	
	if(offset < man->par->to){ 		 //We could shift lseek directly in 'to' position and check if it gives a error.    
	   strcpy(man->par->buffer,"ERRORE: l'offset è troppo grande\0");
	   man->par->error = true;

	}else{
		if (lseek(fd, (off_t) man->par->from, SEEK_SET) == -1)       
	    perror("lseek error");   
	
		if (bufferSize != read(fd, bufferFile, bufferSize))    
	        perror("read error"); 

	    bufferFile[bufferSize]='\0';	 
		strcpy(man->par->buffer,bufferFile); 
	}
    serializeParameters(buffer, man->par); 
    write(man->connectfd, &buffer, sizeof(buffer)); 
    
	close(fd);
    return NULL;
}
   
void* writeThread(void* arg){ 
    return NULL;
} 

void* dimThread(void* arg){ 
    
	management *man = ((management *) arg);
    unsigned char buffer[1000]; 

    strcpy(man->par->buffer,"EOO");
	man->par->choice=123;
	serializeParameters(buffer, man->par); 
  	write(man->connectfd, &buffer, sizeof(buffer));  

    return NULL;
} 

void* mainThread(void* arg){

    bool flag = true;

    parameters *par = (parameters *)malloc(sizeof(parameters));
	management *man = (management *)malloc(sizeof(management));

    pthread_t threadDim, threadRead, threadWrite;
	int connectfd = *((int *)arg);
	
	man->connectfd = connectfd;

	while(flag){
		
		unsigned char buffer[1000];
        if(0 == read(connectfd, &buffer, sizeof(buffer)))
		   flag = false;
		else{
		   par = deserializeParameters(buffer, par);
	       man->par = par;

        	if(par->choice==1){
	    		pthread_create(&threadDim,NULL,dimThread,man);
		    	pthread_join(threadDim,NULL);
 	 	    }  
     	    if(par->choice==2){
            	pthread_create(&threadRead,NULL,readThread,man);
		     	pthread_join(threadRead,NULL);
		    } 
		    if(par->choice==3){
             	pthread_create(&threadWrite,NULL,writeThread,man);
		    	pthread_join(threadWrite,NULL);
	    	}
		}
	}
	free(man);
	free(arg);
	free(par);
	
	return NULL;
}

int CreateSocket(){

    int socketfd;
    struct sockaddr_in serveraddr;

    socketfd=socket(AF_INET,SOCK_STREAM,0);
    if (socketfd == -1) { 
		perror("Socket creation failed"); 
		exit(0); 
	} 
	else
		printf("Socket successfully created..\n"); 

    serveraddr.sin_family=AF_INET;
    serveraddr.sin_addr.s_addr=INADDR_ANY;
    serveraddr.sin_port=htons(PORT);

    //Bind del socket
    if ((bind(socketfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr))) != 0) { 
		perror("Socket bind failed"); 
		exit(0); 
	} 
	else
		printf("Socket successfully binded..\n");   

    // Now server is ready to listen and verification 
	if ((listen(socketfd, 5)) != 0) { 
		perror("Listen failed"); 
		exit(0); 
	} 
	else
		printf("Server listening..\n"); 

    return socketfd;
}


int main(){

	signal(SIGPIPE, SIG_IGN);

	int socketfd, connectfd, lenght; 
	struct sockaddr_in client;
	pthread_t threadMain;	

    socketfd = CreateSocket();
	lenght = sizeof(client); 

    while(1){
	    // Accept the data packet from client and verification 
	    connectfd = accept(socketfd, (struct sockaddr*)&client, (socklen_t*)&lenght); 
	    if (connectfd < 0) { 
	     	perror("server acccept failed"); 
	     	exit(0); 
	    } 
     	else
	     	printf("server acccept the client...\n");
    
        int* clientfd=(int*)malloc(sizeof(int));
    	*clientfd=connectfd;

    	pthread_create(&threadMain,NULL,mainThread,clientfd);
      	pthread_detach(threadMain);
	
    }
	// After chatting close the socket 
	close(socketfd);   
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
