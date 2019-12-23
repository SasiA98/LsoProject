#include "client_library.h"



int main(int argc, const char ** argv){ 
    
	signal(SIGPIPE, SIG_IGN);

	int socketfd; 
	struct sockaddr_in servaddr; 

    // socket create and varification 
	socketfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (socketfd == -1) { 
		perror("Socket creation failed"); 
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
	if (connect(socketfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) { 
		perror("Connection with the server failed"); 
		exit(0); 
	}
	else
		printf("Connected to the server..\n"); 

	// function for chat 
	clientFunctions(socketfd); 

	// close the socket 
	close(socketfd); 
} 

