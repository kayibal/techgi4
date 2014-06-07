/*
############################################################################
#                                                                          #
# Copyright TU-Berlin, 2011-2014                                           #
# Die Weitergabe, Veröffentlichung etc. auch in Teilen ist nicht gestattet #
#                                                                          #
############################################################################
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "packing.h"
#include "hashTable.h"
#include "forwarding.h"
#include "fingertable.h"

#define MAX_BUFFER_LENGTH 100
#define MAX_HASH_TABLE_SIZE 256

/**********************************
 
 SERVER MAIN
 
 *********************************/
int main(int argc, char *argv[]) {
    
    struct sockaddr_in their_addr;
    struct hostent *he, *client_he;

    // Define ports and id's
    if (argc != 9) {
        fprintf(stderr,"Usage: ./server nodeId nodePort suc_id suc_port suc_ip pre_id pre_port pre_ip\n");
        exit(1);
    }
    // Node
    int nodeId = atoi(argv[1]);
    int nodePort = atoi(argv[2]);
    // Successor
    int sucId = atoi(argv[3]);
    int sucPort = atoi(argv[4]);
    char* sucIp = argv[5];
    // Predecessor
    int preId = atoi(argv[6]);
    int prePort = atoi(argv[7]);
    char *preIp = argv[8];
    printf("sucPort: %i\n", sucPort);
    // calculate our intervall
    int upper = nodeID;
    int lower = preId +1;
    printf("Node %i started with port %i.\n\n", nodeId, nodePort);
    
    /*
     Build the fingertable here of size log(n) = 8 because n is 256
     for node 0
     0: 1 -> 63
     1: 2 -> 63
     2: 4 -> 63
     3: 8 -> 63
     4: 16 -> 63
     5: 32 -> 63
     6: 64 -> 128
     7: 128 -> 191
     
     
     for node 127
     0: 127 + 1 % 256 = 128 -> 191
     1: 127 + 2 -> 191
     2: 127 + 4 -> 191
     3: 127 + 8-> 191
     4: 127 + 16 -> 191
     5: 127 + 32 = 191 -> 191
     6: 127 + 64  = 1 -> 63
     7: 127 + 128 % 256 = 1 -> 63
     
     we have four nodes so our node ids are
     0 contains keys 192 - 255 and 0
     63 contains keys 1-63
     127 contains keys 64 - 127
     191 contains keys 128 - 191
     
     
     0 . . . . . . . 63
     .                .
     .                .
     .                .
     .                .
     191 . . . . . . 128
     
    */
    
    //build Fingertable
    int m = 8;
    buildFingerTable(m);
    for (int i = 0; i < m; i++){
        //calculate next node
        int nId = (nodeId + 2^i)%2^m
        //TODO
        //check if node exists hmmm....??? Das ist in der Aufgabenstellung sehr schwammig wie wir das machen sollen...
        if(exists){
            //get ip and port and build finger
            addFinger(finger);
        } else {
            //search for next biggest node and build finger
            addFinger(finger);
        }
    }
    
    // Create Hashtable
    createTable();

    // Create the socket
    int sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    if(sockfd == -1) {
        printf("Error: No socket created. \n");
        return EXIT_FAILURE;
    }

    // Setup transport address
    their_addr.sin_family = AF_INET;     
    their_addr.sin_port = htons(nodePort);
    their_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);

    // Binding
    if(bind(sockfd, (struct sockaddr*)(&their_addr), sizeof(their_addr)) <= -1) {
        printf("Error: Binding failed. \n");
        return EXIT_FAILURE;
    }

    //Receiving
    unsigned char buffer[14];
    clearData(buffer);

    struct sockaddr_in recvAddr;
    struct sockaddr_in clientAddr;
    int recvAddrSize = sizeof(recvAddr);
    while(recvfrom(sockfd, &buffer, 14, 0, (struct sockaddr*)(&recvAddr), &recvAddrSize) > 0) {

        //Unpack Data
        unsigned char order[4];
        unsigned int key;
        unsigned int value;
        char clientIp[15];
        unsigned int clientPort;

        //Unpacking
        unpackData(buffer, order, &key, &value, clientIp, &clientPort);

        // Forwarding
        /* We should divide the keys in intervalls and map these to the nodes
         * siehe oben
         */
        /*
        if((key % 4) != nodeId){
            printf("Need to forward... key: %u target: %i\n", key, (key % 4));

            if(forwarding(buffer, sucIp, sucPort) == -1){
                printf("Forwarding failed...");
                return EXIT_FAILURE;
            }

            clearData(buffer);
            continue;
        }
         
*/      //do we need to forward?
        if (upper - lower < 0){
            // special case
            // one intervall must contain the 0
            // Just check between lower and 256
            // and 0 and upper
            if( !((lower < key && key < 256) && (0 < key && key <= upper)) ){
                finger* forwardTo = getFinger(key);
                if(forwarding(buffer, forwardTo->ip, forwardTo->port) == -1){
                    printf("Forwarding failed...");
                    return EXIT_FAILURE;
                }
                clearData(Buffer);
                continue;
            }
        } else {
            if( !(lower < key && key <= upper) ){
                finger* forwardTo = getFinger(key);
                if(forwarding(buffer, forwardTo->ip, forwardTo->port) == -1){
                    printf("Forwarding failed...");
                    return EXIT_FAILURE;
                }
            }
            clearData(Buffer);
            continue;
        }
        if((client_he = gethostbyname(clientIp)) == NULL) {  // get the host info
            herror("gethostbyname");
        }

        clientAddr.sin_family = AF_INET;
        clientAddr.sin_port = htons(clientPort);
        clientAddr.sin_addr = *((struct in_addr *)client_he->h_addr);
        memset(clientAddr.sin_zero, '\0', sizeof clientAddr.sin_zero);
        int clientAddrSize = sizeof(clientAddr);
        
        unsigned char sendBuffer[14];
        clearData(sendBuffer);
        
        // GET
        if(strcmp(order, "GET\0") == 0) {
            int sendValue = -1;
            get(key, &sendValue);
            if(sendValue == -1)
                packData(sendBuffer, "NOF\0", key, value, clientIp, nodePort);
            else
                packData(sendBuffer, "VAL\0", key, sendValue, clientIp, nodePort);
        }
        // SET
        else if(strcmp(order, "SET\0") == 0) {
            if(set(key, value) == -1)
                packData(sendBuffer, "ERR\0", key, value, clientIp, nodePort);
            else
                packData(sendBuffer, "OK!\0", key, value, clientIp, nodePort);
        }
        // DELETE
        else if(strcmp(order, "DEL\0") == 0) {
            int delValue = -1;
            delete(key, &delValue);
            if(delValue == -1)
                packData(sendBuffer, "ERR\0", key, 0, clientIp, nodePort);
            else
                packData(sendBuffer, "OK!\0", key, delValue, clientIp, nodePort);
        }

        // Send to Client
        if(sendto(sockfd, sendBuffer, 14, 0, (struct sockaddr*)(&clientAddr), clientAddrSize) == -1) {
            printf("Error: Sending failed. \n\tIP: %s\n\tPort: %i\n\n", clientIp, clientPort);
            return EXIT_FAILURE;
        }
        else {
            printf("Answer successfully sent to client! \n\tIP: %s\n\tPort: %i\n\n", clientIp, clientPort);
        }
    }
    destroyFingerTable();
    return 0;
}
