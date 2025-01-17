#include "client_library.h"


int main(int argc, const char ** argv){

	signal(SIGPIPE,hendler);
	signal(SIGINT,hendler);
	signal(SIGTERM,hendler);
	signal(SIGILL,hendler);
	signal(SIGQUIT,hendler);


	int err;
	if ((err=checkArgsInvalidClient(argc, argv))) {
    	printf("Usage: %s <IP address/domain name> <TCP port>\n", argv[0]);
    	return err;
  	}


	int socketfd; 
	struct sockaddr_in servaddr; 

    socketfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (socketfd == -1) { 
		perror("Creazione della socket fallita\n"); 
		exit(0); 
	} 
	
	// assegnazione IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = inet_addr(IP_PORT);
	servaddr.sin_port = htons(PORT);

	// connessione client  - server tramite socket
	if (connect(socketfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) { 
		perror("Connessione con il server fallita\n"); 
		exit(0); 
	}
	else
		printf("Connesso al server...\n"); 


	printf("\n\n----------------------- LSO PROJECT -----------------------\n\n");

	clientFunctions(socketfd); 
	
	// chiusura della socket 
	close(socketfd); 

	return 0;
} 

