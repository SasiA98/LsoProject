#include <unistd.h>
#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <signal.h>
#include <sys/socket.h> 
#include <arpa/inet.h>
#define MAX 80 
#define PORT 8080 
#define SA struct sockaddr
#define IP_PORT "127.0.0.1"


typedef struct intero {
	int num;
}intero;

void func(int sockfd){ 
	intero* peppe=(intero*)malloc(sizeof(intero));
	for (;;) {  
		printf("Enter the numb : ");
		scanf("%d",&peppe->num);
		send(sockfd,peppe,sizeof(peppe),0);
		
		recv(sockfd, peppe, sizeof(peppe),0);
		printf("\nFrom Server : %d\n", peppe->num);
		
		if(!peppe->num){
			free(peppe);
			break;
		}
	} 
} 

int main(){ 

	signal(SIGPIPE, SIG_IGN);

	int sockfd; 
	struct sockaddr_in servaddr; 

	// socket create and varification 
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
	//inet_aton("127.0.0.1", &servaddr.sin_addr);
	servaddr.sin_addr.s_addr = inet_addr(IP_PORT);
	servaddr.sin_port = htons(PORT);

	// connect the client socket to server socket 
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
		printf("connection with the server failed...\n"); 
		exit(0); 
	}
	else
		printf("connected to the server..\n"); 

	// function for chat 
	func(sockfd); 

	// close the socket 
	close(sockfd); 
} 
