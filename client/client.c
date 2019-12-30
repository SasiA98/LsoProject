#include "client_library.h"


int main(int argc, const char ** argv){

//---------DA CANCELLARE-----------
	argc=3;
	argv[1]="127.0.0.1";
	argv[2]="8080";

//---------------------------------

	signal(SIGPIPE,hendler);
	signal(SIGINT,hendler);
	signal(SIGTERM,hendler);
	signal(SIGILL,hendler);
	signal(SIGQUIT,hendler);

    printf("\n\n----------------------- LSO PROJECT -----------------------\n\n");

	int err;
	if ((err=checkArgsInvalidClient(argc, argv))) {
    	printf("Usage: %s <IP address> <TCP port>\n", argv[0]);
    	return err;
  	}

	int socketfd; 
	struct sockaddr_in servaddr; 

    socketfd = createSocket(servaddr);
	
	// connessione cllient  - server tramite socket
	if (connect(socketfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) { 
		perror("Connessione con il server fallita"); 
		exit(0); 
	}
	else
		printf("Connesso al server..\n"); 

	printf("\n\n");	
	clientFunctions(socketfd); 
	
	// chiusura della socket 
	close(socketfd); 

	return 0;
} 

