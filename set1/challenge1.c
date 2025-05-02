#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//We want a function which takes as it's argument an string in hex representation and spits out it's base64 encoding

//First of all we define an auxiliary function which will convert a 2 digit hex string into it's byte representation

unsigned char hex2byte(char *hex) {
    int len = strlen(hex);
    unsigned char result = 0;

    for (int i = 0; i < 2 && i < len ; i++) { //Want to make sure we handle the case of only 1 character too
        char c = hex[i];
        result *= 16;
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

//Now we make a function which converts from any hex to it's byte representation

unsigned char * hex2bytes(char *input, int len) {
    unsigned char * bytes = (unsigned char *) malloc((len + 1) / 2); //roundup the number of bytes we need to store input
    if (!bytes) return NULL;

    for (int i = 0; i < (len + 1) / 2; i++) {
        bytes[i] = hex2byte(input);
        input += 2;
    }
    return bytes;
}

//Now it's time for our actual base64 encoder

char * hex2base64(char * input) {

    int len = strlen(input); // we assume len is a multiple of 6, call it 6*m
    unsigned char * bytes = hex2bytes(input, len); //bytes has size 3m
    char * lookup = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
    char * result = (char *) malloc(2 * (len / 3) + 1); // 3m bytes -> 24m bits -> 4m base64 chars

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

int main() {
    char * input = "49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d";
    char * output = hex2base64(input);
    printf("The base64 representation is %s", output);
}