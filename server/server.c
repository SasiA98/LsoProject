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

#define PORT 8080

typedef struct {
	int choice;
    int from;
    int to;
    int dimFile;
    char buffer[100];
}parameters;


typedef struct management{
    int fd;
    parameters par;
}management;
 

 unsigned char * serialize_int(unsigned char *buffer, int value);
unsigned char * serialize_char(unsigned char *buffer, char value);
int deserialize_int(unsigned char *buffer);
char deserialize_char(unsigned char *buffer);
unsigned char* serializeParameters(unsigned char* msg, parameters par);
parameters deserializeParameters(unsigned char* buffer, parameters par);


void* readThread(void* args){ 
    return NULL;
} 

void* writeThread(void* val){ 
    return NULL;
} 

void* dimThread(void* arg){ 
    
	management *man = ((management *) arg);
    
	man->par.choice=123;
	
    unsigned char buffer[100]; //Controllare i tipi : sasy
	serializeParameters(buffer, man->par); 
  	write(man->fd, &buffer, sizeof(buffer));

    return NULL;
} 

void* mainThread(void* arg){

	int connfd = *((int *)arg);

	pthread_t threadDim, threadRead, threadWrite;

	management *man = (management *)malloc(sizeof(management));
	man->fd = connfd;
    parameters par;

	while(1){
		
		unsigned char buffer[100];
        read(connfd, &buffer, sizeof(buffer));
		par = deserializeParameters(buffer, par);
		
		man->par = par;

    	printf("The choice is: %d\n", par.choice);

    	if(par.choice==1){
			pthread_create(&threadDim,NULL,dimThread,man);
			pthread_join(threadDim,NULL);
		}
    	if(par.choice==2){
        	pthread_create(&threadRead,NULL,readThread,man);
			pthread_join(threadRead,NULL);
		} 
		if(par.choice==3){
        	pthread_create(&threadWrite,NULL,writeThread,man);
			pthread_join(threadWrite,NULL);
		}
		if(par.choice==0){
			free(man);
			free(arg);
			break;
			} 
	}

	// GESTIRE SE INPUT DIVERSO DA 1/2/3 : Sasy
	return NULL;
}

int CreateSocket(){

    int socketfd;
    struct sockaddr_in serveraddr;

    socketfd=socket(AF_INET,SOCK_STREAM,0);
    if (socketfd == -1) { 
		printf("Socket creation failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully created..\n"); 

    serveraddr.sin_family=AF_INET;
    serveraddr.sin_addr.s_addr=INADDR_ANY;
    serveraddr.sin_port=htons(PORT);

    //  errore=fcntl(sock,F_SETFL,O_NONBLOCK); server non bloccante 

    //Bind del socket
    if ((bind(socketfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr))) != 0) { 
		printf("Socket bind failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully binded..\n");   

    // Now server is ready to listen and verification 
	if ((listen(socketfd, 5)) != 0) { 
		printf("Listen failed...\n"); 
		exit(0); 
	} 
	else
		printf("Server listening..\n"); 


    return socketfd;
}


int main(){

	signal(SIGPIPE, SIG_IGN);

	int socketfd, connfd, lenght; 
	struct sockaddr_in client;
	pthread_t threadMain;	

    socketfd = CreateSocket();
	lenght = sizeof(client); 

  while(1){
	// Accept the data packet from client and verification 
	connfd = accept(socketfd, (struct sockaddr*)&client, (socklen_t*)&lenght); 
	if (connfd < 0) { 
		printf("server acccept failed...\n"); 
		exit(0); 
	} 
	else
		printf("server acccept the client...\n");
    
    int* val=(int*)malloc(sizeof(int));
	*val=connfd;
	pthread_create(&threadMain,NULL,mainThread,val);
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





/*
// Function designed for chat between client and server. 
void* WriteThread(void* val){ 
	
	int sockfd=(*(int*)val);
    parameters* par=(parameters*)malloc(sizeof(parameters));
	// infinite loop for chat 
	for (;;) {

		// read the message from client and copy it in buffer 
		recv(sockfd, par, sizeof(par),0); 
		// print buffer which contains the client contents 
		printf("From client: %d\n", par->choice);
		// copy server message in the buffer
        int a=peppe->num;
		peppe->num=a*a;
		
		printf("From server: %d\n", peppe->num);
		// and send that buffer to client 
		send(sockfd, peppe, sizeof(peppe),0); 

		// if msg contains "Exit" then server exit and chat ended. 
		if(!peppe->num){
			free(val);
			free(peppe);
			break;
		}
		
	} 
	
	return NULL;
} 
*/