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

    parameters par;

	while(1) {  
		printf("Inserisci 1 se vuoi conoscere la dimensione del file\nInserisci 2 per leggere da file\nInserisci 3 per scrivere su file\nInserisci 0 per uscire\nInserito: ");
		
		par.choice=getInt();

		while(par.choice<0 || par.choice>3){
			printf("Valore fuori dal range!\nInserire di nuovo:");
			par.choice=getInt();
		}

		send(socketfd,&par,sizeof(par),0);

		if(par.choice==0){
			printf("Esco.\n");
			//free(dataParameters);
			break;
		}

		switch (par.choice){
			case 1: dimension(socketfd);
			break;
			case 2: readFile(socketfd);
			break;
			case 3: writeFile(socketfd);
			break;
		}


		
		/*
		send(sockfd,parameters,sizeof(parameters),0);
		
		recv(sockfd, parameters, sizeof(parameters),0);
		printf("\nFrom Server : %d\n", parameters->num);
		*/
	}
}

void dimension(int socketfd){

	parameters par;
	recv(socketfd, &par, sizeof(par),0);
	int dim=par.dimFile; //La lettura di dimFile riesce : Genny
//	free(par);
    printf("SI: %d\n\n",par.dimFile); 
    printf("SI: %d\n\n",dim); 

}

void readFile(int socketfd){
    printf("Qui si legge dal file.\n");

}

void writeFile(int socketfd){
    printf("Qui si scrive sul file.\n");

}