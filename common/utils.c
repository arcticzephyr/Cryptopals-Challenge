// utils.c
#include "utils.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function definitions

//challenge 1 

unsigned char hex2byte(char *hexStr) {
    unsigned char result = 0;
    for (int i = 0; i < 2 ; i++) { //Want to make sure we handle the case of only 1 character too
        char c = hexStr[i];
        result *= 16;

        if (c == '\0') break;
            
        if ('0' <= c && c <= '9') {
            result += c - '0';
        } else if ('A' <= c && c <= 'F') {
            result += 10 + c - 'A';
        } else {
            result += 10 + c - 'a';
        }
    }
    return result;
}

unsigned char * hex2bytes(char *input, int len) {
    unsigned char * bytes = (unsigned char *) malloc((len + 1) / 2); // Add space for null terminator
    if (!bytes) { 
        return NULL;
    }

    for (int i = 0; i < (len + 1) / 2; i++) {
        bytes[i] = hex2byte(input);
        input += 2;
    }
    return bytes;
}

char * hex2base64(char * input) {

    int len = strlen(input); // we assume len is a multiple of 6, call it 6*m
    unsigned char * bytes = hex2bytes(input, len); //bytes has size 3m
    char * lookup = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
    char * result = (char *) malloc(2 * (len / 3) + 1); // 3m bytes -> 24m bits -> 4m base64 chars
    if (!result) {
        free(bytes);
        return NULL;
    }

    for (int i = 0; i < len / 6; i++) {

        result[4 * i]     = lookup[(bytes[3 * i] >> 2) & 0x3F];
        result[4 * i + 1] = lookup[((bytes[3 * i] & 0x03) << 4) | ((bytes[3 * i + 1] >> 4) & 0x0F)];
        result[4 * i + 2] = lookup[((bytes[3 * i + 1] & 0x0F) << 2) | ((bytes[3 * i + 2] >> 6) & 0x03)];
        result[4 * i + 3] = lookup[bytes[3 * i + 2] & 0x3F];
    }
    free(bytes);
    result[(2 * len) / 3] = '\0';
    return result;
}

int findIndex(char * str, char element) {
    int index = 0;
    if (str == NULL) return -1;
    while (str[index] != '\0') {
        if (str[index] == element) {
            return index;
        } else {
            index++;
        }
    }
    return -1;
}

unsigned char * base642bytes(char * input) {
    char * lookup = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
    int len = strlen(input);
    unsigned char * bytes = malloc((3 * len) / 4 + 1);
    for (int i = 0; i < len / 4; i++) {
        unsigned char char1 = findIndex(lookup, input[4 * i]);
        unsigned char char2 = findIndex(lookup, input[4 * i + 1]);
        unsigned char char3 = findIndex(lookup, input[4 * i + 2]);
        unsigned char char4 = findIndex(lookup, input[4 * i + 3]);

        unsigned char byte1 = (char1 << 2) | (char2 >> 4);
        unsigned char byte2 = ((char2 & 0x0F) << 4) | (char3 >> 2);
        unsigned char byte3 = ((char3 & 0x03) << 6) | char4;

        bytes[3 * i] = byte1;
        if (input[4 * i + 2] != '=') bytes[3 * i + 1] = byte2;
        if (input[4 * i + 3] != '=') bytes[3 * i + 2] = byte3;
    }
    bytes[(3 * len) / 4] = '\0';
    return bytes;
} 


//challenge2

unsigned char * bytes2hex(unsigned char * bytes, int len) { //len is the size of the bytes array     
    unsigned char * hex = (unsigned char *) malloc(2 * len + 1);
    const char * hexlookup = "0123456789abcdef";
    for (int i = 0; i < len; i++) {
        hex[2 * i] = hexlookup[(bytes[i] & 0xf0) >> 4];
        hex[2 * i + 1] = hexlookup[(bytes[i] & 0x0f)];
    }
    hex[2*len] = '\0';
    return hex;
}

int max(int a, int b) {
    return a > b ? a : b;
}

int min(int a, int b) {
    return a < b ? a : b;
}


unsigned char * singlebyteXOR(unsigned char * bytes, unsigned char byte, int len) {
    unsigned char * newbytes = (unsigned char *)malloc(len);
    if (!newbytes) return NULL; // Handle memory allocation failure
    for (int i = 0; i < len; i++) {
        newbytes[i] = bytes[i] ^ byte;
    }
    return newbytes;
}

//challenge 3 

void freqcalc(unsigned char * bytes, float * freqprobs, int len) {
    //We calculate the frequency ratio of each character
    for (int i = 0; i < 128; i++) {
        freqprobs[i] = 0.0;
    } //re-initalise array
    for (int i = 0; i < len; i++) {
        if (bytes[i] < 128) {
            freqprobs[bytes[i]] += 1.0 / len;
        }
    }
}

float cipherscore(unsigned char * encoded, int len) {

    unsigned char example[] = "It was a bright cold day in April, "
"and the clocks were striking thirteen. Winston Smith, his chin nuzzled into his breast "
"in an effort to escape the vile wind, slipped quickly through the glass doors of Victory Mansions, "
"though not quickly enough to prevent a swirl of gritty dust from entering along with him."; //Excerpt from 1984

    float freqprobs[128] = {0};
    float expectedfreqprobs[128] = {0};

    freqcalc(example, expectedfreqprobs, len);
    freqcalc(encoded, freqprobs, len);

    float square_error = 0.0;
    for (int i = 0; i < 128; i++) {
        square_error += (freqprobs[i] - expectedfreqprobs[i]) * (freqprobs[i] - expectedfreqprobs[i]);
    }
    return square_error;
}

unsigned char findSingleXORKey(unsigned char * encoded, int len) {
    unsigned char best_key;
    float currerror;
    float minerror = 1000.0;
    unsigned char * curr;

    for (int i = 0; i < 256; i++) {
        unsigned char *temp = singlebyteXOR(encoded, i, len);
        currerror = cipherscore(temp, len);
        if (currerror < minerror) {
            best_key = (unsigned char)i;
            minerror = currerror;
        }
        free(temp);
    }
    return best_key;
}

//Challenge 5

//Repeating Key XOR Encryption: returns a new message

unsigned char * RKXOR(unsigned char * message, unsigned char * key, int meslen, int keylen) { 
    unsigned char * newmessage = malloc(meslen);
    for (int i = 0; i < meslen; i++) {
        newmessage[i] = message[i] ^ key[i % keylen];
    }
    return newmessage;
}

//Take hamming distance of first 'len' bytes of each of str1, str2. Returns -1 if either str1 or str2 too small.

int hammingdistance(unsigned char * str1, unsigned char * str2, int len) {
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    if ( (len1 < len)||(len2 < len) ) {
        return -1;
    }
    int distance = 0;
    for (int i = 0; i < len; i++) {
        unsigned char dummy = str1[i] ^ str2[i];
        for (int j = 0; j < 8; j++) {
            distance += (dummy >> j) & 0x01;
        }
    }
    return distance;
}

//Challenge 7

//Multiplication in GF(2^8)

unsigned char gf_multiply(unsigned char a, unsigned char b) {
    unsigned char result = 0;
    while (b) {
        if (b & 1) result ^= a;
        a = (a << 1) ^ ((a & 0x80) ? 0x1b : 0x00); 
        b >>= 1;
    }
    return result;
}

//Affine transformation that gets applied after inverting to yield S-Box lookup

unsigned char affine_transform(unsigned char x) {
    unsigned char result = 0;
    for (int i = 0; i < 8; i++) {
        result |= (
            ((x >> i) & 1) ^
            ((x >> ((i + 4) % 8)) & 1) ^
            ((x >> ((i + 5) % 8)) & 1) ^
            ((x >> ((i + 6) % 8)) & 1) ^
            ((x >> ((i + 7) % 8)) & 1)
        ) << i;
    }
    return result ^ 0x63;
}

void shift(unsigned char * word) {
    unsigned char temp = word[0];
    word[0] = word[1];
    word[1] = word[2];
    word[2] = word[3];
    word[3] = temp;
}

//Now we build a general shift function

void shiftby(unsigned char * word, int n) {
    n = ((n % 4) + 4) % 4; // Ensure n is non-negative
    for (int i = 0; i < n; i++) {
        shift(word);
    }
}


