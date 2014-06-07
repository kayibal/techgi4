#ifndef OWNHASHTABLE_H
#define OWNHASHTABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


#define MAX_BUFFER_LENGTH 100 //Wozu??
#define MAX_HASH_TABLE_SIZE 256

typedef struct {
    int set;
    int key;
    int value;
} tableField;

int freeTableFields = MAX_HASH_TABLE_SIZE;
tableField hashTable[MAX_HASH_TABLE_SIZE];

/*------------------------------------------
------------Server functions----------------
--------------------------------------------*/

void createTable() {
    int i;
    for(i = 0; i < MAX_HASH_TABLE_SIZE; i++) {
        tableField current;
        current.set = 0;
        current.value = 0;
        current.key = 0;
        hashTable[i] = current;
    }
}



int set(int key, int value) {

    // Hashtable is full
    if(freeTableFields == 0)
        return EXIT_FAILURE;

    // Get free field
    int hash = key % MAX_HASH_TABLE_SIZE;
    while(hashTable[hash].set == 1 && hashTable[hash].value != value) {
        hash = (hash + 1) % MAX_HASH_TABLE_SIZE;
        if(hash == key % MAX_HASH_TABLE_SIZE)
            return EXIT_FAILURE;
    }
    
    // Set item values
    hashTable[hash].set = 1;
    hashTable[hash].key = key;
    hashTable[hash].value = value;
    
    freeTableFields--;
    printf("SET: set: %i\n", hashTable[hash].set);
    printf("SET: key: %i\n", hashTable[hash].key);
    printf("SET: value: %i\n\n", hashTable[hash].value);
    return EXIT_SUCCESS;
}



int get(int key, unsigned int *value) {
    
    // Table is empty
    if(freeTableFields == MAX_HASH_TABLE_SIZE)
        return EXIT_FAILURE;
    
    // Search field with the same key
    int hash = key % MAX_HASH_TABLE_SIZE;
    while (hashTable[hash].key != key) {
        hash = (hash + 1) % MAX_HASH_TABLE_SIZE;
        if(hash == key % MAX_HASH_TABLE_SIZE)
            return EXIT_FAILURE;
    }

    if(hashTable[hash].set == 0) {
        return EXIT_FAILURE;
    }

    *value = hashTable[hash].value;
    return EXIT_SUCCESS;
}



int delete(int key, unsigned int* val) {
    
    // Table is empty
    if(freeTableFields == MAX_HASH_TABLE_SIZE) {
        printf("Table is empty!\n");
        return EXIT_FAILURE;
    }
    
    // Search field with the same key
    int hash = key % MAX_HASH_TABLE_SIZE;
    while (hashTable[hash].key != key) {
        hash = (hash + 1) % MAX_HASH_TABLE_SIZE;
        if(hash == key % MAX_HASH_TABLE_SIZE)
            return EXIT_FAILURE;
    }

    if(hashTable[hash].set == 0) {
        printf("Nothing to delete!\n");
        return EXIT_FAILURE;
    }
    
    *val = hashTable[hash].value;

    // Delete value
    hashTable[hash].set = 0;
    hashTable[hash].value = -1;
    freeTableFields++;
    printf("Deleted value '%i' from key '%i'!\n", hashTable[hash].value, key);
    return EXIT_SUCCESS;
}



#endif //OWNHASHTABLE_H