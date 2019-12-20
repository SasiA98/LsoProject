#include <unistd.h>
#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <stdbool.h>
#define MAX 80
#define PORT 8080 
#define SA struct sockaddr
#define IP_PORT "127.0.0.1"


typedef unsigned int uint;

typedef struct parameters {
	int choice;
    int from;
    int to;
    int dimFile;
    char *buffer[];
}parameters;



int getInt();
void func(int sockfd);
int dimension(int socketfd);
void getStr(char * str, uint len);



int main(){ 

	signal(SIGPIPE, SIG_IGN);

	int socketfd; 
	struct sockaddr_in servaddr; 

	// socket create and varification 
	socketfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (socketfd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully created..\n"); 
	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	//inet_aton("127.0.0.1", &servaddr.sin_addr);
	servaddr.sin_addr.s_addr = inet_addr(IP_PORT);
	servaddr.sin_port = htons(PORT);

	// connect the client socket to server socket 
	if (connect(socketfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
		printf("connection with the server failed...\n"); 
		exit(0); 
	}
	else
		printf("connected to the server..\n"); 

	// function for chat 
	func(socketfd); 

	// close the socket 
	close(socketfd); 
} 

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

void func(int socketfd){ 

	parameters* dataParameters=(parameters*)malloc(sizeof(parameters));

	while(1) {  
		printf("Inserisci 1 se vuoi conoscere la dimensione del file\nInserisci 2 per leggere da file\nInserisci 3 per scrivere su file\nInserisci 0 per uscire\nInserito: ");
		
		dataParameters->choice=getInt();

		while(dataParameters->choice<0 || dataParameters->choice>3){
			printf("Valore fuori dal range!\nInserire di nuovo:");
			dataParameters->choice=getInt();
		}

		send(socketfd,dataParameters,sizeof(dataParameters),0);

		if(dataParameters->choice==0){
			printf("Esco.\n");
			free(dataParameters);
			break;
		}

		switch (dataParameters->choice){
			case 1: printf("Dimensione:%d\n",dimension(socketfd));
			break;
			case 2: printf("Qui ti faccio leggere da file.\n");
			break;
			case 3: printf("Qui ti faccio inserire la stringa nel file.\n");
			break;
		}


		
		/*
		send(sockfd,parameters,sizeof(parameters),0);
		
		recv(sockfd, parameters, sizeof(parameters),0);
		printf("\nFrom Server : %d\n", parameters->num);
		*/
	}
}

int dimension(int socketfd){

	int* num=(int*)malloc(sizeof(int));

	recv(socketfd, num, sizeof(int),0);

	int dim=*num;

	free(num);

	return dim;
}