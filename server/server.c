#include "server_library.h"

int main(int argc, const char** argv){

//---------DA CANCELLARE-----------
	argc=4;
	argv[1]="8080";
	argv[2]="../file/Document.txt";
	argv[3]="1000";

//---------------------------------

	signal(SIGPIPE,SIG_IGN);
	signal(SIGINT, handler);
	signal(SIGQUIT, handler);
	signal(SIGTERM, handler);
	signal(SIGILL, handler);

	// Controlli all'avvio del server.
	int err;
	if ((err=checkArgsInvalidServer(argc, argv))) {
		switch(err){
			case 1: fprintf(stderr,"Usage: %s <TCP port> <Name File> <Max Dimension>\n", argv[0]);
				break;
			case 2: fprintf(stderr,"Porta TCP fuori dal range!\n");
				break;
			case 3: fprintf(stderr,"Path del file troppo grande!\n");
				break;
			case 4: fprintf(stderr,"Il file %s non esiste!\n",argv[2]);
				break;
			case 5: fprintf(stderr,"Non si hanno i diritti di lettura per il file!\n");
				break;
			case 6: fprintf(stderr,"Non si hanno i diritti di scrittura per il file!\n");
				break;
			case 7: fprintf(stderr,"Apertura del file fallita!\n");
				break;
			case 8: fprintf(stderr,"La dimensione deve essere un valore positivo!\n");
				break;
			case 9: fprintf(stderr,"Dimensione massima inserita inferiore a quella reale!\n");
				break;
		}
    	return err;
  	}

	int socketfd, connectfd, lenght; 
	struct sockaddr_in client;
	pthread_t threadMain;	

    socketfd = CreateSocket();
	lenght = sizeof(client);

	syncro=(syncronize*)malloc(sizeof(syncronize)); 
	syncro->numReader=0;

	if(pthread_mutex_init(&(syncro->mutexWrite),NULL)){
		perror("Inizializzazione Mutex fallita");
		exit(0);
	}

	if(pthread_mutex_init(&(syncro->mutexRead),NULL)){
		perror("Inizializzazione Mutex fallita");
		exit(0);
	}

    while(1){ 
	    connectfd = accept(socketfd, (struct sockaddr*)&client, (socklen_t*)&lenght); 
	    if (connectfd < 0) { 
	     	perror("Server accept fallita"); 
	     	exit(0); 
	    } 
        int* clientfd=(int*)malloc(sizeof(int));
    	*clientfd=connectfd;
		
    	if(pthread_create(&threadMain,NULL,mainThread,clientfd)!=0){
			perror("Creazione thread fallita");
			exit(0);
		}
      	if(pthread_detach(threadMain)!=0){
			  perror("Thread detach fallita");
			  exit(0);
		}	
    }

	close(socketfd);
	return 0;
} 