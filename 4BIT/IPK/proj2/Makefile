CC = g++
CFLAGS = -w -std=c++11 -static-libstdc++

all: ipk-mtrip

ipk-mtrip:  ipk-mtrip.o 
	$(CC) $(CFLAGS) -o ipk-mtrip ipk-mtrip.o

ipk-mtrip.o:  ipk-mtrip.cpp
	$(CC) $(CFLAGS) -c ipk-mtrip.cpp


clean: 
	rm -f ipk-mtrip *.o
