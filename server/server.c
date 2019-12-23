#include "server_library.h"


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