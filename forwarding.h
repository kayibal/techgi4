#ifndef FORWARDING_H
#define FORWARDING_H

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


int forwarding(char* buffer, char* sucIp, int sucPort) {
     

    printf("FORWARDING\n");
    struct sockaddr_in their_addr;
    struct hostent *he;
    int sockfd;
     
     
    // Create socket
    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    if(sockfd == -1) {
        printf("Error: No socket created.\n");
        return -1;
    }
     
   // Resolve hostname to IP Address
    if((he = gethostbyname(sucIp)) == NULL) {  // get the host info
        herror("gethostbyname");
    }


    // Setup transport address
    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(sucPort);
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    long addr_size = sizeof(their_addr);
    memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);
     
    // Connect
    if(connect(sockfd, (struct sockaddr*)(&their_addr), sizeof(their_addr)) == -1) {
        printf("Error: Connecting failed.\n");
        return -1;
    }
    printf("Connected to server.\n");

    //Send
    if(sendto(sockfd, buffer, 14, 0, (struct sockaddr*)(&their_addr), addr_size) == -1) {
        printf("Error: Forwarding failed.");
        return -1;
    }
    printf("Forwarded... \n");

    //Close
    if(close(sockfd) == -1) {
        printf("Error: Closing failed.");
        return -1;
    }

    return 0;
}


#endif //FORWARDING_H