#include "server_library.h"

int dimension;
int PORT;
char nameFile[MAX_DIM_NAME_FILE];

int checkArgsInvalidServer(int argc, const char *argv[]){
    if (argc!=4) {
        return 1;
    }

    // First argument must be positive (and other conditions to implements): genny
    char *p = NULL;
    int a = (int) strtol(argv[1], &p, 10);
    if (p == NULL || *p != '\0'|| a<0 || a>65535) 
        return 4;

    // Second argument must exist
    int fd = open(argv[2], O_WRONLY);
    if (fd<0 || strlen(argv[2])>=MAX_DIM_NAME_FILE-1) {
        return 3;
    }
	int dim;
	if ((dim=lseek(fd, 0, SEEK_END)) == -1){
        perror("cannot seek\n");
    }
    close(fd);


    // Third argument must be positive (and other conditions to implements): genny
    int b = (int) strtol(argv[3], &p, 10);
    if (p == NULL || *p != '\0'|| b<dim) 
        return 4;

    PORT=a;
    strncpy(nameFile,argv[2],MAX_DIM_NAME_FILE-1);
    dimension=b;

    return 0;
}

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

void* writeThread(void* arg){   
	management *man = ((management *) arg);

	int fd, bufferSize;
	unsigned char buffer[DIM_PARAMETERS];

    if ((fd = open(nameFile, O_WRONLY)) == -1) //Does it need any permission? : sasy    
	    perror("open error");   

	pthread_mutex_lock(&mutex);

    if(dimension < (man->par->from + strlen(man->par->buffer))){
	    strncpy(man->par->buffer,"ERRORE: Il file raggiunge una dimensione non consentita\0",DIM_BUFFER-1);
		man->par->error = 1;  
	}else{
		if (lseek(fd, (off_t) man->par->from, SEEK_SET) == -1)       
	        perror("lseek error");   

        bufferSize = strlen(man->par->buffer);
		if (bufferSize != write(fd, man->par->buffer, bufferSize))    
	        perror("write error"); 
	
	    strncpy(man->par->buffer,"OK\0",DIM_BUFFER-1); //Valore non letto dal client.
	}

	pthread_mutex_unlock(&mutex);

    serializeParameters(buffer, man->par); 
    write(man->connectfd,(void*) &buffer, sizeof(buffer)); 
    
	close(fd);
    return NULL;
}
   
void* readThread(void* arg){ 
   	management *man = ((management *) arg);

	int fd, offset;
	char bufferFile[DIM_PARAMETERS];
	unsigned char buffer[DIM_PARAMETERS];

	int bufferSize = man->par->to - man->par->from; // when read from keyboard check that from is smaller then to : sasy OK

    if ((fd = open(nameFile, O_RDONLY)) == -1) //Does it need any permission? : sasy    
	    perror("open error");   
  
    if ((offset = lseek(fd, (off_t) 0, SEEK_END)) == -1)
	     perror("lseek error");

	if(bufferSize >= DIM_BUFFER){
	   strncpy(man->par->buffer,"ERRORE: La richiesta ha superato la dimensione massima.\0",DIM_BUFFER-1);
	   man->par->error = 3;

	}else if(offset == 0){
	   strncpy(man->par->buffer,"ERRORE: Il File è vuoto\0",DIM_BUFFER-1);
	   man->par->error = 2;

	}else if(offset < man->par->to){ 	  
	   strncpy(man->par->buffer,"ERRORE: l'offset è troppo grande\0",DIM_BUFFER-1);
	   man->par->error = 1;
	   
	}else{
		if (lseek(fd, (off_t) man->par->from, SEEK_SET) == -1)       
	    perror("lseek error");   
	
		pthread_mutex_lock(&mutex);

		if (bufferSize != read(fd, bufferFile, bufferSize))    
	        perror("read error");

		pthread_mutex_unlock(&mutex);

	    bufferFile[bufferSize]='\0';	 
		strncpy(man->par->buffer,bufferFile,DIM_BUFFER-1); // Probabile segmantation fault usare strncpy al posto di strcpy : Genny
	}
    serializeParameters(buffer, man->par); 
    write(man->connectfd,(void*) &buffer, sizeof(buffer)); 
    
	close(fd);
    return NULL;
} 

void* dimThread(void* arg){ 
    
	management *man = ((management *) arg);
    unsigned char buffer[DIM_PARAMETERS]; 

    int fd, dim=0;

    if ((fd = open(nameFile, O_RDONLY)) == -1){ //Does it need any permission? : sasy    
	    perror("open error");
        man->par->error=1;
        strncpy(man->par->buffer,"errore accesso file\n",DIM_BUFFER-1);
    }

	pthread_mutex_lock(&mutex);

    if ((dim=lseek(fd, 0, SEEK_END)) == -1){
        perror("cannot seek\n");
        man->par->error=1;
    }
    else{
        man->par->dimFile=dim;
    }

	pthread_mutex_unlock(&mutex);
    
	
	serializeParameters(buffer, man->par); 
  	write(man->connectfd,(void*) &buffer, sizeof(buffer)); 

    close(fd); 

    return NULL;
} 
