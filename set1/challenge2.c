#include "../common/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char * hexXOR(char * hex1, char * hex2) {
    int len1 = strlen(hex1);
    int len2 = strlen(hex2);
    unsigned char * bytes1 = hex2bytes(hex1, len1);
    unsigned char * bytes2 = hex2bytes(hex2, len2);
    unsigned char * result = (unsigned char *) malloc((max(len1,len2) + 1) / 2);
    for (int i = 0; i < (max(len1,len2) + 1) / 2; i++) {
        result[i] = bytes1[i] ^ bytes2[i]; //XOR the two byte arrays
    }
    return bytes2hex(result, (max(len1, len2) + 1) / 2);
}

int main() {
    char hex1[] = "1c0111001f010100061a024b53535009181c";
    char hex2[] = "686974207468652062756c6c277320657965";
    printf("The answer is %s\n",hexXOR(hex1,hex2));
}