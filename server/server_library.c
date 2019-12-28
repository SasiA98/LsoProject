#include "server_library.h"

int dimension;
int PORT;
char nameFile[MAX_DIM_NAME_FILE];


void handler(int signal){
	
	pthread_mutex_destroy(&(syncro->mutexWrite));
	pthread_mutex_destroy(&(syncro->mutexRead));
	free(syncro);
	perror("Fine del programma");
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
        perror("errore lseek");
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
		perror("Creazione della socket fallita"); 
		exit(0); 
	} 

    serveraddr.sin_family=AF_INET;
    serveraddr.sin_addr.s_addr=INADDR_ANY;
    serveraddr.sin_port=htons(PORT);

    //Bind del socket
    if ((bind(socketfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr))) != 0) { 
		perror("Socket bind fallito"); 
		exit(0); 
	} 

    // Now server is ready to listen and verification 
	if ((listen(socketfd, 5)) != 0) { 
		perror("Listen fallito"); 
		exit(0); 
	} 

    return socketfd;
}

void* mainThread(void* arg){

    bool flag = true;
	int connectfd = *((int *)arg);
    pthread_t threadDim, threadRead, threadWrite;

   	uchar buffer[DIM_BUFFER]={};

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

	int fd, bufferSize, dim;
	uchar buffer[DIM_PARAMETERS]={};

    if ((fd = open(nameFile, O_WRONLY)) == -1) // How does it react if there is an error? : sasi
	    strncpy(man->par->buffer,"ERRORE: Il file non e' stato aperto\0",DIM_BUFFER-1), man->par->error = 1;

	pthread_mutex_lock(&(syncro->mutexWrite));

    if(dimension < (man->par->from + strlen(man->par->buffer))){
	    strncpy(man->par->buffer,"ERRORE: Il file raggiunge una dimensione non consentita\0",DIM_BUFFER-1), man->par->error = 1;
	}else{
		if ((dim=lseek(fd, 0, SEEK_END)) == -1)       
		    strncpy(man->par->buffer,"ERRORE: funzione lseek fallita\0",DIM_BUFFER-1), man->par->error = 1;

		if(dim < man->par->from){
			int lenght=(man->par->from - dim);
			char spaces[lenght];

			for(int i=0;i<lenght;i++){ //Do we have to add 'EOF' if we write after the 'EOF'? :sasi
				spaces[i] = ' ';
			}
			write(fd,spaces,lenght); 
		}

		if (lseek(fd, (off_t) man->par->from, SEEK_SET) == -1)       
		    strncpy(man->par->buffer,"ERRORE: funzione lseek fallita\0",DIM_BUFFER-1), man->par->error = 1;

        bufferSize = strlen(man->par->buffer);
		if (bufferSize != write(fd, man->par->buffer, bufferSize))    
		    strncpy(man->par->buffer,"ERRORE: scrittura su file non corretta\0",DIM_BUFFER-1), man->par->error = 1;
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
	uchar buffer[DIM_PARAMETERS]={};

	int bufferSize = man->par->to - man->par->from; 

    if ((fd = open(nameFile, O_RDONLY)) == -1)
	    strncpy(man->par->buffer,"ERRORE: Il file non e' stato aperto\0",DIM_BUFFER-1), man->par->error = 1;
  
    if(bufferSize >= DIM_BUFFER){
	   strncpy(man->par->buffer,"ERRORE: La richiesta ha superato la dimensione massima.\0",DIM_BUFFER-1);
	   man->par->error = 1;
	}

    if(man->par->error != 3){
        pthread_mutex_lock(&(syncro->mutexRead));
		syncro->numReader++;

    	if ((syncro->numReader)==1){ 
    	    pthread_mutex_lock(&(syncro->mutexWrite));
		}
		pthread_mutex_unlock(&(syncro->mutexRead));
    	
		if ((offset = lseek(fd, (off_t) 0, SEEK_END)) == -1){
		    strncpy(man->par->buffer,"ERRORE: funzione lseek fallita\0",DIM_BUFFER-1), man->par->error = 1;
    
    	 }else if(offset == 0){
	            strncpy(man->par->buffer,"ERRORE: Il File è vuoto\0",DIM_BUFFER-1);
	            man->par->error = 2;

             	}else if(offset < man->par->to){ 	  
    	            strncpy(man->par->buffer,"ERRORE: l'offset è troppo grande\0",DIM_BUFFER-1);
    	            man->par->error = 1;
	   
                	}else{
		                if (lseek(fd, (off_t) man->par->from, SEEK_SET) == -1)       
		                    strncpy(man->par->buffer,"ERRORE: funzione lseek fallita\0",DIM_BUFFER-1), man->par->error = 1;

	                	if (bufferSize != read(fd, bufferFile, bufferSize))    
		                    strncpy(man->par->buffer,"ERRORE: lettura su file non corretta\0",DIM_BUFFER-1), man->par->error = 1;

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

    uchar buffer[DIM_PARAMETERS]={}; 

    int fd, dim=0;

    if ((fd = open(nameFile, O_RDONLY)) == -1){ 
	    strncpy(man->par->buffer,"ERRORE: Il file non e' stato aperto\0",DIM_BUFFER-1), man->par->error = 1;
    }

	pthread_mutex_lock(&(syncro->mutexRead));
	syncro->numReader++;
	
	if ((syncro->numReader)==1) 
	    pthread_mutex_lock(&(syncro->mutexWrite));
	pthread_mutex_unlock(&(syncro->mutexRead));

    if ((dim=lseek(fd, 0, SEEK_END)) == -1){
		strncpy(man->par->buffer,"ERRORE: funzione lseek fallita\0",DIM_BUFFER-1), man->par->error = 1;
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
