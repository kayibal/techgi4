//
//  fingertable.h
//  
//
//  Created by Alan Höng on 06/06/14.
//
//

#ifndef _fingertable_h
#define _fingertable_h

typedef struct {
    char* ip;
    unsigned int port;
    int lower;
    int upper;
} finger;

unsigned int tableSize = 0;
unsigned int tPointer = 0;
finger *fingertable = NULL;

int buildFingerTable(unsigned int chordSize);

int addFinger(char* ip, unsigned int port, int lower, int upper);

finger* getFinger(int key);

void destroyFingerTable();
#endif
