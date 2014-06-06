/*
############################################################################
#                                                                          #
# Copyright TU-Berlin, 2011-2014                                           #
# Die Weitergabe, Verรถffentlichung etc. auch in Teilen ist nicht gestattet #
#                                                                          #
############################################################################
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "packing.h"

int sockfd, recv_socket;
struct sockaddr_in my_addr, chord_addr;
int binded = 0;

typedef struct {    
    char *ip;
    unsigned int port;
} peerAddress;
peerAddress clientAddr;
peerAddress chordAddr;



int hashTableSet(int key, int value) {
    unsigned char answer[4];
    unsigned int getValue;
    unsigned int getKey;
    char serverIp[15];
    int serverPort;

    printf("Setting value '%i' at key '%i'...\n", value, key);
    if(sendAndGet("SET\0", answer, key, value, &getKey, &getValue, &serverIp, &serverPort) == -1) {
        printf("ERROR: Setting value '%i' at key '%i' failed!\n\n", value, key);
        return EXIT_FAILURE;
    }
    else {
        printf("\tAnswer: %s\n\tValue: %u\n\tKey: %u\n\tIP: %s\n\tPort: %i\n\n", answer, getValue, getKey, serverIp, serverPort);
        // printf("Setting value '%i' at key '%i'. DONE!\n\n", value, key);
        return EXIT_SUCCESS;
    }
}


// REMOTE GET
int hashTableGet(int key) {
    unsigned char answer[4];
    unsigned int getValue;
    unsigned int getKey;
    char serverIp[15];
    int serverPort;

    printf("Getting value from key '%i'...\n", key);
    if( -1 == sendAndGet("GET\0", answer, key, 0, &getKey, &getValue, &serverIp, &serverPort)) {
        printf("ERROR: Getting value from key '%i' failed!\n\n", key);
        return EXIT_FAILURE;
    }
    else {
        printf("\tAnswer: %s\n\tValue: %u\n\tKey: %u\n\tIP: %s\n\tPort: %i\n\n", answer, getValue, getKey, serverIp, serverPort);
        return EXIT_SUCCESS;
    }
}
 
 
// REMOTE DELETE
int hashTableDelete(int key) {
    unsigned char answer[4];
    unsigned int getValue;
    unsigned int getKey = 0;
    char serverIp[15];
    int serverPort;

    printf("Deleting field with key '%i'...\n", key);
    if(-1 == sendAndGet("DEL\0", answer, key, 0, &getKey, &getValue, serverIp, &serverPort)) {
        printf("ERROR: Deleting field with key '%i' failed!\n\n", key);
        return EXIT_FAILURE;
    }
    else {
        printf("\tAnswer: %s\n\tValue: %u\n\tKey: %u\n\tIP: %s\n\tPort: %i\n\n", answer, getValue, getKey, serverIp, serverPort);
        return EXIT_SUCCESS;
    }
}

 
// SEND DATA - RECEIVE DATA
int sendAndGet(unsigned char *order, unsigned char *answer, unsigned int key, 
    unsigned int sendValue, unsigned int *getKey, unsigned int *getValue, 
    char *serverIp, unsigned int *serverPort) {
     
    // Pack Data
    unsigned char buffer[14];
    clearData(buffer);
    packData(buffer, order, key, sendValue, clientAddr.ip, clientAddr.port);

    // Send
    if(send(sockfd, buffer, 14, 0) == -1) {
        printf("Error: Sending failed. \n");
        return EXIT_FAILURE;
    }

    // Receive
    unsigned char recvBuffer[14];
    struct sockaddr_in recvAddr;
    int recvAddrSize = sizeof(recvAddr);
    if(recvfrom(recv_socket, recvBuffer, 14, 0, (struct sockaddr*)(&recvAddr), &recvAddrSize) == -1) {
        printf("Error: Receiving failed. \n");
        return EXIT_FAILURE;
    }
    unpackData(recvBuffer, answer, getKey, getValue, serverIp, serverPort);
    return 0;
}
 

// CONNECT TO REMOTE SERVER
int connectToChord() {
    struct hostent *chord_he;
     
    // Resolve hostname to IP Address
    if((chord_he = gethostbyname(chordAddr.ip)) == NULL) {
        herror("gethostbyname");
    }
     
    // Create sockets
    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    recv_socket = socket(PF_INET, SOCK_DGRAM, 0);
    if(sockfd == -1) {
        printf("Error: No socket created.\n");
    }
    if(recv_socket == -1) {
        printf("Error: No socket created.\n");
    }
     
    // Setup transport address
    chord_addr.sin_family = AF_INET;
    chord_addr.sin_port = htons(chordAddr.port);
    chord_addr.sin_addr = *((struct in_addr *)chord_he->h_addr);
    memset(chord_addr.sin_zero, '\0', sizeof chord_addr.sin_zero);
    
    // Setup receive address
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(clientAddr.port);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(my_addr.sin_zero, '\0', sizeof my_addr.sin_zero);

    // Binding for receiving
    if(bind(recv_socket, (struct sockaddr*)(&my_addr), sizeof(my_addr)) == -1) {
        printf("Error: Binding failed. \n");
        return EXIT_FAILURE;
    }
     
    // Connect
    if(connect(sockfd, (struct sockaddr*)(&chord_addr), sizeof(chord_addr)) == -1) {
        printf("Error: Connecting failed.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}