client : client.o client_library.o ../protocol/protocol.o 
	gcc -Wall -o client client.o client_library.o ../protocol/protocol.o 

client.o : client.c client_library.h 
	gcc -Wall -c client.c

client_library.o : client_library.c ../protocol/protocol.h
	gcc -Wall -c client_library.c

protocol.o : ../protocol/protocol.c
	gcc -Wall -c ../protocol/protocol.c

