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
#include "clientStub.h"

int main(int argc, char *argv[]) {
    if(argc != 5){
        printf("Usage: ./client clientIP clientPort chordIp cordPort\n");
        exit(1);
    }
    //Time measurement start
    unsigned long start, end;
    start = current_time_millis();
    
    // Define client address
    clientAddr.ip = argv[1];
    clientAddr.port = atoi(argv[2]);
    // Define known chord address
    chordAddr.ip = argv[3];
    chordAddr.port = atoi(argv[4]);

    // Connect to known chord
    if(connectToChord() == -1) {
        printf("Error: Sending data failed. \n");
        return EXIT_FAILURE;
    }

    // Create random values
    int i;
    int max = 3;
    srand(time(NULL));
    int values[max];
    for (i = 0; i < max; i++) {
        values[i] = rand() % 65535;
    }

    // HASHTABLE: SET
    for (i = 0; i < max; i++) {
        int test = hashTableSet(values[i], i);
        if(test == -1) {
            printf("Setting value '%i' at key '%i' failed. \n\n", values[i], i);
            return EXIT_FAILURE;
        }

    }
    printf("\n\n\n\n");
     
    // HASHTABLE: GET
    for (i = 0; i < max; i++) {
        if(hashTableGet(values[i]) == -1) {
            printf("Getting value '%i' failed. \n", values[i]);
            return EXIT_FAILURE;
        }
    }
    printf("\n\n\n\n");
     
    // HASHTABLE: DELETE
    for (i = 0; i < max; i++) {
        if(hashTableDelete(values[i]) == -1) {
            printf("Delete %i failed. \n", values[i]);
            return EXIT_FAILURE;
        }
    }
    printf("\n\n\n\n");
    
    // HASHTABLE: GET
    for (i = 0; i < max; i++) {
        if(hashTableGet(values[i]) == -1) {
            printf("Getting value '%i' failed. \n", values[i]);
            return EXIT_FAILURE;
        }
    }
    
    // print the measured time and average
    end = current_time_millis();
    printf("%.2f seconds\n", (double) (end - start) / 1000);
    printf("Average: %.2f seconds\n", ((double) (end - start) / 1000) / (max*3));

    return EXIT_SUCCESS;
}
