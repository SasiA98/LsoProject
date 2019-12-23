#include "server_library.h"



int CreateSocket(){

    int socketfd;
    struct sockaddr_in serveraddr;

    socketfd=socket(AF_INET,SOCK_STREAM,0);
    if (socketfd == -1) { 
		perror("Socket creation failed\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully created...\n"); 

    serveraddr.sin_family=AF_INET;
    serveraddr.sin_addr.s_addr=INADDR_ANY;
    serveraddr.sin_port=htons(PORT);

    //Bind del socket
    if ((bind(socketfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr))) != 0) { 
		perror("Socket bind failed\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully binded...\n");   

    // Now server is ready to listen and verification 
	if ((listen(socketfd, 5)) != 0) { 
		perror("Listen failed\n"); 
		exit(0); 
	} 
	else
		printf("Server listening...\n"); 

    return socketfd;
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

void* readThread(void* arg){   //I thinked to make an infinit cicle to wait the correct input of the client, but if the client dies, the server remains in loop and the management of mutex is more difficult : sasi
	management *man = ((management *) arg);

	int fd, offset;
	char bufferFile[1000];
	unsigned char buffer[1000];

	int bufferSize = man->par->to - man->par->from; // when read from keyboard check that from is smaller then to : sasy 

    if ((fd = open("../file/Document.txt", O_RDONLY)) == -1) //Does it need any permission? : sasy    
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
	man->par->dimFile=123;
	serializeParameters(buffer, man->par); 
  	write(man->connectfd, &buffer, sizeof(buffer));  

    return NULL;
} 
