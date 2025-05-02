#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../common/utils.h"

int main() {
    char message[] = "Burning 'em, if you ain't quick and nimble\nI go crazy when I hear a cymbal";
    char key[] = "ICE";
    RKXOR(message, key);
    char * encryptedmessage;
    encryptedmessage = bytes2hex(message, strlen(message));
    RKXOR(message, key);
    printf("When the string %s is XOR'D against the key %s, we get %s", message, key, encryptedmessage);
    free(encryptedmessage);
}