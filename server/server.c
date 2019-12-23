#include "server_library.h"


int main(int argc, const char** argv){

	int err;
	if ((err=are_args_invalid(argc, argv))) {
    	printf("Usage: %s <TCP port> <Name File> <Max Dimension>\n", argv[0]);
    	return err;
  	}



	int socketfd, connectfd, lenght; 
	struct sockaddr_in client;
	pthread_t threadMain;	

    socketfd = CreateSocket();
	lenght = sizeof(client); 

    while(1){
	    // Accept the data packet from client and verification 
	    connectfd = accept(socketfd, (struct sockaddr*)&client, (socklen_t*)&lenght); 
	    if (connectfd < 0) { 
	     	perror("Server accept failed\n"); 
	     	exit(0); 
	    } 
     	else
	     	printf("Server accept the client...\n");
    
        int* clientfd=(int*)malloc(sizeof(int));
    	*clientfd=connectfd;

    	if(pthread_create(&threadMain,NULL,mainThread,clientfd)!=0){
			perror("Thread creation failed\n");
			exit(0);
		}
      	if(pthread_detach(threadMain)!=0){
			  perror("Thread detach failed\n");
			  exit(0);
		}
	
    }


	close(socketfd);
} 