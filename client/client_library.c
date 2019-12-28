#include "client_library.h"

void hendler(int signum){
    printf("SIGPIPE error, connection lost...\n");
    exit(0);
}

int checkArgsInvalidClient(int argc, const char *argv[]){
    if (argc!=3) {
        return 1;
    }

    // Second argument must be positive (and other conditions to implements): genny
    char *p = NULL;
    int a = (int) strtol(argv[2], &p, 10);
    if (p == NULL || *p != '\0'|| a<0 || a>65535) 
        return 4;

    // First argument
    if (strlen(argv[1])>=19) {
        return 3;
    }

    strncpy(IP_PORT,argv[1],19);
    PORT=a;

    return 0;
}

void getStr(char * str, uint len){
  assert(str != NULL);
  uint i;
  char c;
  for(i = 0; (i < len) && ((c = getchar()) != '\n') && (c != EOF); ++i)
    {
      str[i] = c;
    }
  str[i] = '\0';
}

int getInt(){
    int num;
    bool vuoto;
    char* fine;
    char* temp=(char*)malloc(sizeof(char)* 30);
    do {
        getStr(temp, 30);

        if(strlen(temp)==0) vuoto=true;
        else vuoto=false;

        num = strtol(temp, &fine, 10);
        if(*fine!='\0' || vuoto) printf("\nValore errato, inserire di nuovo:");
    }while (*fine!='\0' || vuoto);
    free(temp);
    return  num;
}

void faultyConnection(int numRequest, parameters *par){ //Evaluate the max value that numRequest can reach
    if(numRequest==par->numRequest){
			fprintf(stderr,"La connessione e' caduta");
        exit(1);
    }
}

void clientFunctions(int socketfd){ 
	parameters *par =(parameters *)malloc(sizeof(parameters)); 

	while(1) {  
		printf("Inserisci 1 se vuoi conoscere la dimensione del file\nInserisci 2 per leggere da file\nInserisci 3 per scrivere su file\nInserisci 0 per uscire\nInserito: ");
		par->choice=0;
        par->choice=getInt(); // if I press cntr+d it goes into a loop :sasi

		while(par->choice<0 || par->choice>3){
			printf("Valore fuori dal range!\nInserire di nuovo:");
			par->choice=getInt(); // If I insert a bigger value than 10 byte, the program cycles until the length of the input ends
 		}
        
		if(par->choice==0){
			printf("Esco.\n");
			exit(0);
		}

		switch (par->choice){
			case 1: dimension(socketfd,par);
			break;
			case 2: readFile(socketfd,par);
			break;
			case 3: writeFile(socketfd,par);
			break;
		}
	}
}

void dimension(int socketfd, parameters *par){
    
    unsigned char bufferR[DIM_PARAMETERS], bufferW[DIM_PARAMETERS];
    par->error = 0; 
    int numRequest = par->numRequest;

    serializeParameters(bufferW, par); 
    write(socketfd,&bufferW,sizeof(bufferW)); //We should manage the cases in which the 'write' returns -1

	read(socketfd, &bufferR, sizeof(bufferR));
	deserializeParameters(bufferR, par);

    faultyConnection(numRequest,par);

    if(par->error==0){
        printf("Dimensione:%d\n",par->dimFile);
    }
    else{
        printf("%s",par->buffer);
    }
    
}

void writeFile(int socketfd,parameters *par){
    
    unsigned char bufferR[DIM_PARAMETERS], bufferW[DIM_PARAMETERS]; //Does it need clear the buffer when the client do a new request?
    do 
    {
        par->error = 0; 
        int numRequest = par->numRequest;

        do
        {
            printf("Inserisci l'indice da cui partire\n");
            printf("From: ");
            par->from=getInt(); 
            printf("Inserisci stringa da scrivere (MAX %d): ", DIM_BUFFER);
            getStr(par->buffer,DIM_BUFFER-1); //If stdin get over dim_buffer (?)
        
            if(par->from < 0) 
               printf("Errore: from dev'essere necessariamente maggiore o uguale di 0\n");

        } while (par->from <0); 
    
        serializeParameters(bufferW, par); 
        write(socketfd,&bufferW,sizeof(bufferW));

    	read(socketfd, &bufferR, sizeof(bufferR));
    	deserializeParameters(bufferR, par);  
       
        faultyConnection(numRequest,par);

        if(par->error != 0){
            printf("%s\n",par->buffer);
        }
        else   
           printf("La stringa e' stata inserita correttamente\n"); 

    } while (par->error != 0);
}

void readFile(int socketfd, parameters *par){
    
    bool flag = true;
    unsigned char bufferR[DIM_PARAMETERS], bufferW[DIM_PARAMETERS]; //Does it need clear the buffer when the client do a new request?
    do 
    {
        par->error = 0; 
        int numRequest = par->numRequest;

        do
        {
            printf("Inserisci il range da leggere (MAX %d)\n",DIM_BUFFER);
            printf("From: ");
            par->from=getInt(); 
            printf("To: ");
            par->to=getInt(); 
        
            if(par->from > par->to || par->from < 0) 
               printf("Errore: from dev'essere necessariamente maggiore o uguale di to e maggiore di 0\n");

        } while (par->from > par->to || par->from <0); 
    
        serializeParameters(bufferW, par); 
        write(socketfd,&bufferW,sizeof(bufferW));

    	read(socketfd, &bufferR, sizeof(bufferR));
    	deserializeParameters(bufferR, par);
                 
        faultyConnection(numRequest,par);

        if(par->error != 0){
            printf("%s\n",par->buffer);
            if(par->error == 2)
                flag = false;
        }
        else   
           printf("Ecco la stringa letta dal file :\n%s\n\n",par->buffer); 
  
      } while (par->error != 0 && flag == true);
}
