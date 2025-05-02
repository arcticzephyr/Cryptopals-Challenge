#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../common/utils.h"

//Find KEYSIZE function >

float * findKS(char * encodedstring, int length) {
    char * pointer = encodedstring;
    float * keysizeerror = calloc(40, sizeof(float));
    float dist;
    float running = 0;
    int num_blocks = 0;
    for (int keysize = 1; keysize < 41; keysize++) {
        running = 0;
        num_blocks = 0;
        for (int i = 0; i < length / keysize; i++ ) {
            dist = (float)hammingdistance(pointer + i * keysize, pointer + (i + 1) * keysize, keysize);
            if (dist != -1) {
                running += dist / (float)keysize;
                num_blocks += 1;
            }
        }
        keysizeerror[keysize - 1] = running / (num_blocks + 1);
    }
    return keysizeerror;
}

//Now we decrypt using mostly stuff from earlier challenges

unsigned char * RKXORkeyfind(unsigned char * encoded, int keysize, int length) {
    unsigned char ** blocks = (unsigned char **) malloc(keysize * sizeof(unsigned char *)); // blocks[i] will point to the string which consists of the (i + keysize * k )th char in the string for all k.
    int remainder = length % keysize;

    for (int i = 0; i < keysize; i++) { //We take some care to ensure each block gets exactly as much memory it needs
        blocks[i] = (unsigned char *) malloc((length / keysize + (i < remainder ? 1:0)) * sizeof(unsigned char)); 
    }

    for (int j = 0; j < length; j++) {
        blocks[j % keysize][j / keysize] = encoded[j];
    }

    unsigned char *fullkey = malloc(keysize);

    for (int i = 0; i < keysize; i++) {
        fullkey[i] = findSingleXORKey(blocks[i], (length / keysize + (i < remainder ? 1:0)));
        free(blocks[i]);
    }
    free(blocks);
    return fullkey;
}

int main() {
    FILE *file = fopen("6.txt", "r");

    char line[256];
    char * message = malloc(10000); //We allocate 10000 bytes to be safe
    char * ptr = message;

    while (fgets(line, sizeof(line), file)) {
        int len = strlen(line);
        if (line[len - 1] == '\n') len --;
        memcpy(ptr, line, len);
        ptr += len;
    }
    *ptr = '\0';
    
    int length = strlen(message);
    unsigned char * messagebytes = base642bytes(message);
    unsigned char * key;
    unsigned char * decoded;
    
    for (int i = 2; i < 40; i++) {
        key = RKXORkeyfind(messagebytes, i, (3 * length) / 4);
        decoded = RKXOR(messagebytes,key, (3 * length) / 4 + 1, i);
        printf("For keysize %i unencoded message is %s\n",i,decoded);
        free(key);
        free(decoded);
    }
    free(messagebytes);
    free(message);
}
