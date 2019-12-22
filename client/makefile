client : client.o client_library.o 
	gcc -Wall -o client client.o client_library.o 

client.o : client.c client_library.h 
	gcc -Wall -c client.c

client_library.o : client_library.c
	gcc -Wall -c client_library.c

