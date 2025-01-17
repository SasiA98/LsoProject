#include "client_library.h"

parameters* par;


void hendler(int signal){
    perror("Fine del programma\n");
	exit(1);
}


int checkArgsInvalidClient(int argc, const char *argv[]){
    if (argc!=3) {
        return 1;
    }
    
    struct hostent *host = gethostbyname(argv[1]);


    if(host!=NULL){
        struct in_addr *addr = (struct in_addr *) host->h_addr_list[0];
		strcpy(IP_PORT , inet_ntoa(*addr) );
        
    }else{
         herror("Error gethostbyname");
         return 4;
    }


    // Controllo che il secondo argomento rispetti i vincoli.
    char *p = NULL;
    int a = (int) strtol(argv[2], &p, 10);
    if (p == NULL || *p != '\0'|| a<0 || a>65535) 
        return 3;

    PORT=a;


    return 0;
}


void getStr(char * str, uint len){ //Funzione per la lettura da tastiera di stringhe.
    assert(str != NULL);
    uint i;
    char c;
    for(i = 0; (i < len) && ((c = getchar()) != '\n') && (c != EOF); ++i){
      str[i] = c;
    }
    while ((i>=len) && (c = getchar()) != '\n' && c != EOF) { }
    str[i] = '\0';
}


int getInt(){ //Funzione per la lettura da tastiera di numeri.
    int num;
    bool vuoto;
    char* fine;
    char* temp=(char*)malloc(sizeof(char)* 30);
    do {
        getStr(temp, 30);

        if(strlen(temp)==0) vuoto=true;
        else vuoto=false;

        num = strtol(temp, &fine, 10);
        if(*fine!='\0' || vuoto) printf("Valore errato, riprova:  ");
    }while (*fine!='\0' || vuoto);
    free(temp);
    return  num;
}


void faultyConnection(int numRequest, parameters *par){
    if(numRequest==par->numRequest){
		fprintf(stderr,"La connessione e' caduta");
        free(par);
        exit(1);
    }
}


void clientFunctions(int socketfd){ 
	par =(parameters *)malloc(sizeof(parameters)); 
    par->numRequest=0;
    par->dimFile=0;
    par->error=0;
    par->from=0;
    par->to=0;
    strcpy(&(par->buffer[0]),"0");
    
	
    while(1) {  
   
		printf("1)  Dimensione del file\n2)  Leggere da file\n3)  Scrivere su file\n0)  Exit\n\nScelta:  ");
		par->choice=0;
        par->choice=getInt();

		while(par->choice<0 || par->choice>3){
			printf("Valore fuori dal range, riprova:  ");
			par->choice=getInt();
 		}
        printf("\n");
        
		if(par->choice==0)
			break;

		switch (par->choice){
			case 1: dimension(socketfd,par);
			break;
			case 2: readFile(socketfd,par);
			break;
			case 3: writeFile(socketfd,par);
			break;
		}
	}

    free(par);
}


void dimension(int socketfd, parameters *par){
    
    uchar bufferR[DIM_PARAMETERS]={}, bufferW[DIM_PARAMETERS]={};
    par->error = 0; 
    int numRequest = par->numRequest;

    serializeParameters(bufferW, par);
    write(socketfd,bufferW,sizeof(bufferW)); 
    
    if(read(socketfd, bufferR, sizeof(bufferR))>0)
        deserializeParameters(bufferR, par);

    faultyConnection(numRequest,par);

    if(par->error==0){
        printf("Dimensione:  %d\n\n",par->dimFile);
    }
    else{
        printf("%s\n\n",par->buffer);
    }
    
}


void writeFile(int socketfd,parameters *par){
    
    uchar bufferR[DIM_PARAMETERS]={}, bufferW[DIM_PARAMETERS]={}; 
    do 
    {
        par->error = 0; 
        int numRequest = par->numRequest;

        do
        {
            printf("Inserisci la posizione da cui iniziare a scrivere\n");
            printf("From: ");
            par->from=getInt(); 
            printf("Inserisci contenuto (MAX %d): ", DIM_BUFFER-2);
            getStr(par->buffer,DIM_BUFFER-1);
        
            if(par->from < 0) 
               printf("Errore: 'from' dev'essere maggiore o uguale di '0'\n\n");

        } while (par->from <0); 
    
        serializeParameters(bufferW, par); 
        write(socketfd,bufferW,sizeof(bufferW));

        if(read(socketfd, bufferR, sizeof(bufferR))>0)
            deserializeParameters(bufferR, par);
       
        faultyConnection(numRequest,par);

        if(par->error != 0){
            printf("%s\n\n",par->buffer);
        }
        else   
           printf("La stringa e' stata inserita correttamente\n\n"); 

    } while (par->error != 0);
}


void readFile(int socketfd, parameters *par){
    
    bool flag = true;
    uchar bufferR[DIM_PARAMETERS]={}, bufferW[DIM_PARAMETERS]={}; 
    do 
    {
        par->error = 0; 
        int numRequest = par->numRequest;

        do
        {
            printf("Inserisci il range da leggere (MAX %d)\n",DIM_BUFFER-1);
            printf("From: ");
            par->from=getInt(); 
            printf("To: ");
            par->to=getInt(); 
        
            if(par->from > par->to || par->from < 0) 
               printf("Errore:  'from' dev'essere maggiore di 'to' e maggiore di '0' \n\n");

        } while (par->from > par->to || par->from <0); 
    
        serializeParameters(bufferW, par); 
        write(socketfd,bufferW,sizeof(bufferW));
    	
        if(read(socketfd, bufferR, sizeof(bufferR))>0)
            deserializeParameters(bufferR, par);
      
        faultyConnection(numRequest,par);
       
        if(par->error != 0){
            printf("%s\n\n",par->buffer);
            if(par->error == 2)
                flag = false;
        }
        else   
           printf("Contenuto:\n%s\n\n",par->buffer); 
  
      } while (par->error != 0 && flag == true);
}