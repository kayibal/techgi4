#Makefile for udpClient

#Compiler
CC	= gcc
CFLAGS	= -I

#Binaries
SERVER	= server
CLIENT	= client

COMMON	= packing.h

all: $(SERVER) $(CLIENT)

#Chord
$(SERVER): hashTable.h forwarding.h $(COMMON)
	$(CC) -o server server.c hashTable.h forwarding.h $(COMMON)

#Client
$(CLIENT): client.c $(COMMON)
	$(CC) -o client client.c clientStub.h $(COMMON)

.PHONY: clean rebuild

clean: 
	rm -f server client

rebuild: clean all