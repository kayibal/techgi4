//
//  fingertable.c
//  
//
//  Created by Alan Höng on 06/06/14.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int buildFingerTable(unsigned int chordSize){
    if (chordsize == 0){
        perror("No empty table allowed! Size must be greater 0");
        return EXIT_SUCCESS;
    }
    finger = malloc(sizeof(finger)*chordSize);
    tableSize = chordSize;
    if (finger){
        perror("Memory Error could not allocate space for fingertable");
        return EXIT_SUCCESS;
    } else {
        printf("Fingertable with size %d successfully initialized", chordSize);
        return EXIT_FAILURE;
    }
}

int addFinger(char* ip, unsigned int port, int lower, int upper){
    if (tPointer < tableSize){
        fingertable[tPointer].ip = ip;
        fingertable[tPointer].port = port;
        fingertable[tPointer].lower = lower;
        fingertable[tPointer].upper = upper;
        tPointer++;
    } else {
        perror("Table is already full!");
        return EXIT_FAILURE;
    }
}

finger* getFinger(unsigned int key){
    for (int i = 0; i < tPointer, i++){
        int lower = fingertable[i].lower;
        int upper = fingertable[i].upper;
        if (upper - lower < 0){
            if ((lower < key && key <= 2^tableSize) && (0 < key && key <= upper)){
                return &fingertable[i];
            }
        } else {
            if (lower < key && key <= upper){
                //right node found return this finger
                return &fingertable[i];
            }
        }
    }
    perror("Finger was not found!");
    return NULL;
}

void destroyFingerTable(){
    free(fingertable);
}