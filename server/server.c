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

	int* num=(int*)malloc(sizeof(int));
	*num=1077;
	printf("From server: %d\n", *num);
	send(man->fd, num, sizeof(int),0);
	free(num);
    
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
	pthread_t thread;	

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
    
    parameters* par=(parameters*)malloc(sizeof(parameters));

    recv(connfd, par, sizeof(par),0);

	management *man = (management *)malloc(sizeof(management));

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
	close(socketfd); 
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