#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../common/utils.h"

int main() {
    unsigned char encodedhex[] = "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736";
    unsigned char * besthex = NULL;
    float currerror;
    float minerror = 1000.0;
    int len = strlen(encodedhex);
    unsigned char * currstr;

    for (int i = 0; i < 256; i++) {
        char temphex[len + 1];
        strcpy(temphex, encodedhex);
        singlebyteXOR(temphex, (unsigned char)i);
        currerror = cipherscore(temphex);
        currstr = hex2bytes(temphex, len);
        if (currerror < minerror) {
            if (besthex) {
                free(besthex);  
                besthex = NULL; 
            }
            int str_len = strlen(currstr) + 1;
            besthex = (unsigned char *)malloc(str_len);
            memcpy(besthex, currstr, str_len);
            minerror = currerror;
        }
        free(currstr);
    }
    printf("The unencoded string is '%s' \n", besthex);
    free(besthex);
}

