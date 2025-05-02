#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../common/utils.h"

#define TABLE_SIZE 10007

//We will naively check for repeated byte blocks in our message, if one comes up(very unlikely to the tune of (0.5)^(128)), we almost surely have detected AES-128 ECB

//First need to implement a hashtable for lookup to check for duplicates

typedef struct blockNode {
    unsigned char block[16];
    struct blockNode * next;
} blockNode;

struct blockNode ** create_hash() {
    struct blockNode ** hash_table = malloc(TABLE_SIZE*sizeof(blockNode *));
    for (int i = 0; i < TABLE_SIZE; i++) {
        hash_table[i] = NULL;
    }
    return hash_table;
}

int hashmap(unsigned char * block) {
    int hashval = 0;
    for (int i = 0; i < 16; i++) {
        hashval += (71 && block[i]);
        hashval *= 307;
        hashval = hashval % TABLE_SIZE;
    }
    return hashval;
}

int hashlookup(unsigned char * block, struct blockNode ** hash_table) {
    int index = hashmap(block);
    struct blockNode * ptr = hash_table[index];
    if (hash_table[index] == NULL) {
        return 0;
    } else {
        while (ptr != NULL) {
            if (memcmp(ptr->block, block, 16) == 0) {
                return 1;
            }
            ptr = ptr->next;
        }
        return 0;
    }
}

void hash_add(unsigned char * block, struct blockNode ** hash_table) {
    if (hashlookup(block, hash_table) == 1) return;

    int index = hashmap(block);
    struct blockNode * ptr = hash_table[index];
    struct blockNode * newNode = malloc(sizeof(struct blockNode));
    memcpy(newNode->block, block,16);
    newNode->next = NULL;

    if (ptr == NULL) {
        hash_table[index] = newNode;
        return;
    }

    while (ptr->next != NULL) {
        ptr = ptr->next;
    }
    ptr->next = newNode;
}

int detectAES(unsigned char * message, int length) {
    struct blockNode ** table = create_hash();
    for (int i = 0; i < length; i+= 16) {
        if (hashlookup(message + i, table) == 1) {
            return 1;
        } else {
        hash_add(message + i, table);
        }
    }
    return 0;
}

int main() {
    FILE*f = fopen("8.txt","r");
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        int length = strlen(line);
        unsigned char * message = hex2bytes(line, length);
        if (detectAES(message, length / 2) == 1) {
            printf("The line that has been encoded is %s",line);
        }
    }
}