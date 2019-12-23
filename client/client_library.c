#include "client_library.h"



void getStr(char * str, uint len)
{
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
    char* temp=(char*)malloc(sizeof(char)* 20);
    do {
        getStr(temp, 10);

        if(strlen(temp)==0) vuoto=true;
        else vuoto=false;

        num = strtol(temp, &fine, 10);
        if(*fine!='\0' || vuoto) printf("\nValore errato, inserire di nuovo:");
    }while (*fine!='\0' || vuoto);
    free(temp);
    return  num;
}

void clientFunctions(int socketfd){ 
	parameters *par =(parameters *)malloc(sizeof(parameters)); // In C the initialization usually is automatic :sasi

	while(1) {  
		printf("Inserisci 1 se vuoi conoscere la dimensione del file\nInserisci 2 per leggere da file\nInserisci 3 per scrivere su file\nInserisci 0 per uscire\nInserito: ");
		par->choice=0;
        par->choice=getInt(); // if I press cntr+d it goes into a loop :sasi

		while(par->choice<0 || par->choice>3){
			printf("Valore fuori dal range!\nInserire di nuovo:");
			par->choice=getInt();
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
    
    unsigned char bufferR[1000], bufferW[1000];

    serializeParameters(bufferW, par); 
    write(socketfd,&bufferW,sizeof(bufferW));

	read(socketfd, &bufferR, sizeof(bufferR));
	par = deserializeParameters(bufferR, par);

    printf("Dimensione:%d\n",par->dimFile);
}

void readFile(int socketfd,parameters *par){
    
    unsigned char bufferR[1000], bufferW[1000]; //Does it need clear the buffer when the client do a new request?
    do 
    {
        par->error = false; 
        do
        {
            printf("Inserisci il range da leggere\n");
            printf("From: ");
            par->from=getInt(); 
            printf("To: ");
            par->to=getInt(); 
        
            if(par->from > par->to || par->from < 0) 
               printf("Errore: from dev'essere necessariamente maggiore o uguale di to e maggiore di 0\n");

        } while (par->from > par->to || par->from <0); //Check the conditions : sasi 
    
        serializeParameters(bufferW, par); 
        write(socketfd,&bufferW,sizeof(bufferW));

    	read(socketfd, &bufferR, sizeof(bufferR));
    	par = deserializeParameters(bufferR, par);  // How do I send a message to the client if the reading is no good? :sasi
       
        if(par->error == true)
           printf("%s\n",par->buffer);
        else   
           printf("Ecco la stringa letta dal file :%s\n",par->buffer); 

    } while (par->error == true);
    

}

void writeFile(int socketfd, parameters *par){
    printf("Qui si scrive sul file.\n");
}
