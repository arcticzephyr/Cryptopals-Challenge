#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../common/utils.h"

//We first build some auxiliary variables which will help later

//The rcon array, which is rcon[i] = 2^(i) in GF(2^8)

const unsigned char rcon[11] = {
    0x00, 
    0x01, 0x02, 0x04, 0x08,
    0x10, 0x20, 0x40, 0x80,
    0x1B, 0x36
};

//We now build the inverse Mix Columns operation - simply multiplying by the inverse of mix columns matrix

void invMixColumns(unsigned char * state) {
    unsigned char temp[16];
    memcpy(temp, state, 16);
    unsigned char matrix[4][4] = {{14,11,13,9},{9,14,11,13},{13,9,14,11},{11,13,9,14}};
    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row ++) {
            // state[row,col] = [sum over dummy] matrix[row, dummy] * state[dummy, col]
            state[row + 4 * col] = 0;
            for (int dummy = 0; dummy < 4; dummy++) {
                state[row + 4 * col] ^= gf_multiply(matrix[row][dummy], temp[dummy + 4 * col]);
            }
        }
    }
}

void invShiftRows(unsigned char * state) {
    unsigned char dum[16];
    memcpy(dum, state, 16); // Copy state to avoid overwriting

    // Row 0: No shift
    state[0] = dum[0];
    state[4] = dum[4];
    state[8] = dum[8];
    state[12] = dum[12];

    // Row 1: Right shift by 1 (equivalent to left shift by 3)
    state[1] = dum[13];  // s[1,0] = s[1,3]
    state[5] = dum[1];   // s[1,1] = s[1,0]
    state[9] = dum[5];   // s[1,2] = s[1,1]
    state[13] = dum[9];  // s[1,3] = s[1,2]

    state[2] = dum[10];  
    state[6] = dum[14];  
    state[10] = dum[2];  
    state[14] = dum[6];  

    state[3] = dum[7];  
    state[7] = dum[11]; 
    state[11] = dum[15];
    state[15] = dum[3];  
}

//First we implement AES-128 Key Expansion 

unsigned char ** keyExpansion(unsigned char * key, unsigned char * sbox) {
    //The key should be 16 bytes, which we split into 4 'words'
    unsigned char ** words = malloc(44 * sizeof(unsigned char *)); // 11 round keys, each round key has 4 words, each word is 4 bytes
    for (int i = 0; i < 44; i++) {
        words[i] = malloc(4);
        if (i < 4) {
            memcpy(words[i], key + 4*i, 4);
        } else if (i % 4 == 0) {
            unsigned char temp[4];
            memcpy(temp, words[i-1], 4);
            shift(temp);
            for (int j = 0; j < 4; j++) {
                temp[j] = sbox[temp[j]];
            }
            temp[0] ^= rcon[i / 4];
            unsigned char * dummy = RKXOR(temp, words[i-4], 4, 4);
            memcpy(words[i], dummy, 4);
            free(dummy);
        } else {
            unsigned char * dummy = RKXOR(words[i-1], words[i-4], 4, 4);
            memcpy(words[i], dummy, 4);
            free(dummy);
        }   
    }
    return words;
}

void addRoundKey(unsigned char * state, unsigned char * roundKey) {
    for (int i = 0; i < 16; i++) {
        state[i] = state[i] ^ roundKey[i];
    }
}

//We will also make a getRoundKey function to avoid having to mess with indices

void getRoundKey(unsigned char *roundKey, unsigned char **words, int round) {
    for (int i = 0; i < 4; i++) {
        memcpy(roundKey + 4*i, words[4*round + i], 4);
    }
}

//Now we build our decryption function, which will act on a 16 byte state

void decryptAESState(unsigned char * state, unsigned char ** words, unsigned char * inverseSbox) {
    unsigned char roundKey[16];
    getRoundKey(roundKey, words, 10);
    addRoundKey(state, roundKey);

    invShiftRows(state);

    for (int j = 0; j < 16; j++) {
        state[j] = inverseSbox[state[j]];
    }

    for (int i = 0; i < 9; i++) {
        getRoundKey(roundKey, words, 9 - i);
        addRoundKey(state, roundKey);

        invMixColumns(state);


        //Now we invert the row shift

        invShiftRows(state);

        //Invert Sbox

        for (int j = 0; j < 16; j++) {
            state[j] = inverseSbox[state[j]];
        }
        
    }
    getRoundKey(roundKey, words, 0);
    addRoundKey(state, roundKey);
} 

void decryptAES(unsigned char * message, unsigned char * key, unsigned char * sbox, unsigned char * inverseSbox, int length) {
    unsigned char ** words = keyExpansion(key, sbox);
    for (int i = 0; i < length; i += 16) {
        decryptAESState(message + i, words, inverseSbox);
    }
    for (int i = 0; i < 44; i++) {
        free(words[i]);
    }
    free(words);
}

int main() {
    char * key = "YELLOW SUBMARINE";

    FILE *f = fopen("7.txt", "r"); //First we read the encoded text file
    char line[256];
    char * message = malloc(20000); 
    char * ptr = message;

    while (fgets(line, sizeof(line), f)) {
        int len = strlen(line);
        if (line[len - 1] == '\n') len --;
        memcpy(ptr, line, len);
        ptr += len;
    }
    *ptr = '\0';
    int length = strlen(message);
    fclose(f);
    unsigned char * messagebytes = base642bytes(message); //We decode it from base64

    //Now we need to read our Sbox and Inverse Sbox arrays
    
    FILE *file = fopen("sbox.bin","rb");
    unsigned char sbox[256];
    unsigned char sboxinverse[256];

    if (fread(sbox, sizeof(unsigned char), 256, file) != 256) {
        fprintf(stderr, "Failed to read sbox\n");
        return 1;
    }
    if (fread(sboxinverse, sizeof(unsigned char), 256, file) != 256) {
        fprintf(stderr, "Failed to read inverse sbox\n");
        return 1;
    }

    decryptAES(messagebytes, key, sbox, sboxinverse, (3 * length) / 4);
    int padding = messagebytes[(3 * length) / 4 - 1];
    messagebytes[(3 * length) / 4 - padding] = '\0';

    // Print or use decrypted data
    printf("The decoded message is %s\n", messagebytes);

    free(message);
    free(messagebytes);
}
