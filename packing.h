#ifndef PACKING_H
#define PACKING_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>


void clearData(unsigned char *buffer) {
    int i;
    for (i = 0; i < 14; ++i) {
        buffer[i] = (char) 0;
    }
    return;
}

void printBuffer(unsigned char *buffer, int max) {
    int i;
    for (i = 0; i < max; ++i) {
        printf("P - buffer[%i]: %u\n", i, buffer[i]);
    }
}

int packData(unsigned char *buffer,
             unsigned char *order,
             unsigned int key,
             unsigned int value,
             char *ip,
             unsigned int port) {

    clearData(buffer);

    // Order
    buffer[0] = order[0];
    buffer[1] = order[1];
    buffer[2] = order[2];
    buffer[3] = order[3];
    // Key
    buffer[4] = (char) (key>>8);
    buffer[5] = (char) (key);
    // Value
    buffer[6] = (char) (value>>8);
    buffer[7] = (char) (value);


    // IP
    // HARDCODED!!!
        //uint32_t ip_htonl = htonl((0xFF << 24) | (0x00 << 16) | (0x00 << 8) | (0x01));
        buffer[8]  = (char) 127;
        buffer[9]  = (char) 0;
        buffer[10] = (char) 0;
        buffer[11] = (char) 1;
    // }
    // else {
    //     printf("PACK - ELSE: %s\n", ip);
    //     buffer[8] = (char) atoi(strtok(ip, "."));
    //     printf("%u\n", buffer[8]);
    //     int i;
    //     for (i = 2; i >= 0; --i) {
    //         buffer[11 - i] = (char) atoi(strtok(NULL, "."));
    //     }
    // }
    // Port
    buffer[12] = (char) (port>>8);
    buffer[13] = (char) (port);

    return 0;
}


void unpackData(unsigned char *buffer, unsigned char *answer, unsigned int *key,
                unsigned int *value, char *ip, unsigned int *port) {

    uint32_t ip_htonl;
    struct in_addr ip_addr;

    answer[0] = buffer[0];
    answer[1] = buffer[1];
    answer[2] = buffer[2];
    answer[3] = buffer[3];
    unsigned int temp_key = (buffer[4]  << 8)  | buffer[5];
    memcpy(key, &temp_key, sizeof(temp_key));
    *value    = (buffer[6]  << 8)  | buffer[7];
    ip_htonl  = (buffer[8]  << 24) | (buffer[9] << 16) | (buffer[10] << 8) | (buffer[11]);
    *port     = (buffer[12] << 8)  | buffer[13];
    ip_addr.s_addr = ntohl(ip_htonl);
    char *temp_ip = (char *) inet_ntoa(ip_addr);
    memcpy(ip, temp_ip, 15);
}

#endif //PACKING_H
