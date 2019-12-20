#include <unistd.h>
#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <signal.h>
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <stdbool.h>
#define MAX 80
#define PORT 8080 
#define SA struct sockaddr
#define IP_PORT "127.0.0.1"


typedef struct intero {
	int num;
	char* s;
}intero;

int getInt();
void func(int sockfd);

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

int getInt(){
    int num;
    bool vuoto;
    char* fine;
    char* temp=(char*)malloc(sizeof(char)* 20);
    do {
        getStr(temp, 20);

        if(strlen(temp)==0) vuoto=true;
        else vuoto=false;

        num = strtol(temp, &fine, 10);
        if(*fine!='\0' || vuoto) printf("\nValore errato, inserire di nuovo:");
    }while (*fine!='\0' || vuoto);
    free(temp);
    return  num;
}

void func(int socketfd){ 
	intero* parameters=(intero*)malloc(sizeof(intero));
	while(1) {  
		printf("Benvenuto\nInserisci 1 se vuoi conoscere la dimensione del file\nInserisci 2 per leggere da file\nInserisci 3 per scrivere su file\nInserisci 0 per uscire\nInserito: ");
		
		parameters->num=getInt();

		if(parameters->num==1){
			printf("Qui ti dico la dimensione.\n");
		}

		if(parameters->num==2){
			printf("Qui ti faccio leggere da file.\n");
		}

		if(parameters->num==3){
			printf("Qui ti faccio inserire la stringa nel file.\n");
		}

		if(parameters->num==0){
			printf("Esco.\n");
			free(parameters);
			break;
		}
		
		/*
		send(sockfd,parameters,sizeof(parameters),0);
		
		recv(sockfd, parameters, sizeof(parameters),0);
		printf("\nFrom Server : %d\n", parameters->num);
		
		if(!parameters->num){
			free(parameters);
			break;
		}
		*/
	} 
} 