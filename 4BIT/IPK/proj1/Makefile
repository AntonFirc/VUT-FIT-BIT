CC = g++
CFLAGS = -w -std=c++11 -static-libstdc++

all: ipk-client ipk-server

ipk-client:  client.o 
	$(CC) $(CFLAGS) -o ipk-client client.o

client.o:  client.cpp
	$(CC) $(CFLAGS) -c client.cpp

ipk-server:  server.o 
	$(CC) $(CFLAGS) -o ipk-server server.o

server.o:  server.cpp
	$(CC) $(CFLAGS) -c server.cpp

clean: 
	rm -f ipk-client ipk-server *.o
