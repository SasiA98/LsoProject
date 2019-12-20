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

#define MAX 30 
#define PORT 8080 
#define SA struct sockaddr 


typedef struct parameters {
	int choice;
    int from;
    int to;
    int dimFile;
    char *buffer[];
}parameters;

typedef struct management{
    int fd;
    parameters *par;
}management;
 

void* ReadThread(void* args){ 
/*	
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
	*/
    return NULL;
} 

void* WriteThread(void* val){ 
       return NULL;
} 

void* DimThread(void* arg){ 
    
    management *man = ((management *) arg); 

	man->par->dimFile = 10;
	printf("From server: %d\n", man->par->dimFile);
	send(man->fd, man->par, sizeof(man->par),0); 
       return NULL;
} 


// Driver function 
int main(){

    management *man = (management *)malloc(sizeof(management));

	signal(SIGPIPE, SIG_IGN);

	int sockfd, connfd, len; 
	struct sockaddr_in servaddr, cli;
	pthread_t thread;	

	// socket create and verification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully created..\n"); 
	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(PORT); 

	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
		printf("socket bind failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully binded..\n"); 

	// Now server is ready to listen and verification 
	if ((listen(sockfd, 5)) != 0) { 
		printf("Listen failed...\n"); 
		exit(0); 
	} 
	else
		printf("Server listening..\n"); 
	len = sizeof(cli); 

  while(1){
	// Accept the data packet from client and verification 
	connfd = accept(sockfd, (SA*)&cli, (socklen_t*)&len); 
	if (connfd < 0) { 
		printf("server acccept failed...\n"); 
		exit(0); 
	} 
	else
		printf("server acccept the client...\n");
    
    parameters* par=(parameters*)malloc(sizeof(parameters));  

    recv(connfd, par, sizeof(par),0); 
    man->fd = connfd;
    man->par = par;

    printf("The choice is: %d\n", par->choice);

     
    if(par->choice==1)
          pthread_create(&thread,NULL,DimThread,man); 
      else if(par->choice==2)	
           pthread_create(&thread,NULL,ReadThread,man); 
        else if(par->choice==3)	
            pthread_create(&thread,NULL,WriteThread,man);

    // GESTIRE SE INPUT DIVERSO DA 1/2/3
    }

	// After chatting close the socket 
	close(sockfd); 
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