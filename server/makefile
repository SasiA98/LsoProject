server : server.o server_library.o ../protocol/protocol.o
	gcc -Wall -pthread -o server server.o server_library.o ../protocol/protocol.o

server.o : server.c server_library.h
	gcc -Wall -c server.c

server_library.o : server_library.c ../protocol/protocol.h
	gcc -Wall -c server_library.c

protocol.o : ../protocol/protocol.c
	gcc -Wall -c ../protocol/protocol.c
