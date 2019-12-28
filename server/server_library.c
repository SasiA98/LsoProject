#include "server_library.h"

int dimension;
int PORT;
char nameFile[MAX_DIM_NAME_FILE];


void handler(int signal){
	
	pthread_mutex_destroy(&(syncro->mutexWrite));
	pthread_mutex_destroy(&(syncro->mutexRead));
	free(syncro);
	perror("end of program");
	exit(1);
	
}

int checkArgsInvalidServer(int argc, const char *argv[]){
	
    if (argc!=4) {
        return 1;
    }

    // First argument must be positive (and other conditions to implements): genny
    char *p = NULL;
    int a = (int) strtol(argv[1], &p, 10);
    if (p == NULL || *p != '\0'|| a<0 || a>65535) 
        return 2;

    // Second argument must exist
	if(strlen(argv[2])>=MAX_DIM_NAME_FILE-1) return 3;

	if(access(argv[2], F_OK)<0) return 4;
	if(access(argv[2], R_OK)<0) return 5;
	if(access(argv[2], W_OK)<0) return 6;

    int fd = open(argv[2], O_WRONLY);
    if (fd<0) return 7;
	int dim;
	if ((dim=lseek(fd, 0, SEEK_END)) == -1){
        perror("cannot seek\n");
    }
    close(fd);


    // Third argument must be positive (and other conditions to implements): genny
    int b = (int) strtol(argv[3], &p, 10);
    if (p == NULL || *p != '\0') return 8;

	if (b<dim) return 9;

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
		perror("Socket creation failed"); 
		exit(0); 
	} 

    serveraddr.sin_family=AF_INET;
    serveraddr.sin_addr.s_addr=INADDR_ANY;
    serveraddr.sin_port=htons(PORT);

    //Bind del socket
    if ((bind(socketfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr))) != 0) { 
		perror("Socket bind failed"); 
		exit(0); 
	} 

    // Now server is ready to listen and verification 
	if ((listen(socketfd, 5)) != 0) { 
		perror("Listen failed"); 
		exit(0); 
	} 

    return socketfd;
}

void* mainThread(void* arg){

    bool flag = true;
	int connectfd = *((int *)arg);
   	unsigned char buffer[DIM_BUFFER]={};
    pthread_t threadDim, threadRead, threadWrite;

    parameters *par = (parameters *)malloc(sizeof(parameters));
	management *man = (management *)malloc(sizeof(management));
	man->connectfd = connectfd;

	while(flag){
		
        if(0 == read(connectfd, buffer, sizeof(buffer)))
		   flag = false;
		else{
		    deserializeParameters(buffer, par);
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
	man->par->numRequest++;

	int fd, bufferSize;
	unsigned char buffer[DIM_PARAMETERS]={};

    if ((fd = open(nameFile, O_WRONLY)) == -1) //Do we notify to client the error of open file?
	    perror("open error");   

	pthread_mutex_lock(&(syncro->mutexWrite));

    if(dimension < (man->par->from + strlen(man->par->buffer))){
	    strncpy(man->par->buffer,"ERRORE: Il file raggiunge una dimensione non consentita\0",DIM_BUFFER-1);
		man->par->error = 1;  
	}else{
		if (lseek(fd, (off_t) man->par->from, SEEK_SET) == -1)       
	        perror("lseek error");   

        bufferSize = strlen(man->par->buffer);
		if (bufferSize != write(fd, man->par->buffer, bufferSize))    
	        perror("write error"); 
	
	    strncpy(man->par->buffer,"OK\0",DIM_BUFFER-1); //Valore non letto dal client, vedere se strncpy lo mette di default 
	}

	pthread_mutex_unlock(&(syncro->mutexWrite));

    serializeParameters(buffer, man->par); 
    write(man->connectfd,(void*) buffer, sizeof(buffer)); 
    
	close(fd);
    return NULL;
}
   
void* readThread(void* arg){ 
   	
	management *man = ((management *) arg);
	man->par->numRequest++;

	int fd, offset;
	char bufferFile[DIM_BUFFER]={};
	unsigned char buffer[DIM_PARAMETERS]={};

	int bufferSize = man->par->to - man->par->from; 

    if ((fd = open(nameFile, O_RDONLY)) == -1)
	    perror("open error");   
  
    if(bufferSize >= DIM_BUFFER){
	   strncpy(man->par->buffer,"ERRORE: La richiesta ha superato la dimensione massima.\0",DIM_BUFFER-1);
	   man->par->error = 3;
	}

    if(man->par->error != 3){
        pthread_mutex_lock(&(syncro->mutexRead));
		syncro->numReader++;

    	if ((syncro->numReader)==1){ 
    	    pthread_mutex_lock(&(syncro->mutexWrite));
		}
		pthread_mutex_unlock(&(syncro->mutexRead));
    	
		if ((offset = lseek(fd, (off_t) 0, SEEK_END)) == -1){
    	    perror("lseek error");
    
    	 }else if(offset == 0){
	            strncpy(man->par->buffer,"ERRORE: Il File è vuoto\0",DIM_BUFFER-1);
	            man->par->error = 2;

             	}else if(offset < man->par->to){ 	  
    	            strncpy(man->par->buffer,"ERRORE: l'offset è troppo grande\0",DIM_BUFFER-1);
    	            man->par->error = 1;
	   
                	}else{
		                if (lseek(fd, (off_t) man->par->from, SEEK_SET) == -1)       
	                        perror("lseek error");

	                	if (bufferSize != read(fd, bufferFile, bufferSize))    
	                        perror("read error");

	                    bufferFile[bufferSize]='\0';	 
	    	            strncpy(man->par->buffer,bufferFile,DIM_BUFFER-1);
                 	} 
 	
        pthread_mutex_lock(&(syncro->mutexRead));
	    syncro->numReader--;
	
     	if ((syncro->numReader)==0){     
        	pthread_mutex_unlock(&(syncro->mutexWrite));
		}
		pthread_mutex_unlock(&(syncro->mutexRead));
	}

    serializeParameters(buffer, man->par); 
    write(man->connectfd,(void*) buffer, sizeof(buffer)); 
    
	close(fd);
    return NULL;
} 

void* dimThread(void* arg){ 
    
	management *man = ((management *) arg);
	man->par->numRequest++;

    unsigned char buffer[DIM_PARAMETERS]={}; 

    int fd, dim=0;

    if ((fd = open(nameFile, O_RDONLY)) == -1){ 
	    perror("open error");
        man->par->error=1;
        strncpy(man->par->buffer,"errore accesso file\n",DIM_BUFFER-1);
    }

	pthread_mutex_lock(&(syncro->mutexRead));
	syncro->numReader++;
	
	if ((syncro->numReader)==1) 
	    pthread_mutex_lock(&(syncro->mutexWrite));
	pthread_mutex_unlock(&(syncro->mutexRead));

    if ((dim=lseek(fd, 0, SEEK_END)) == -1){
        perror("cannot seek");
        man->par->error=2;
    }
    else{
        man->par->dimFile=dim;
    }

	pthread_mutex_lock(&(syncro->mutexRead));
	syncro->numReader--;

	if ((syncro->numReader)==0) 
	    pthread_mutex_unlock(&(syncro->mutexWrite));
	pthread_mutex_unlock(&(syncro->mutexRead));
    

	serializeParameters(buffer, man->par); 
  	write(man->connectfd,(void*) buffer, sizeof(buffer)); 

    close(fd); 

    return NULL;
} 
