#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../common/utils.h"

int main() {
    FILE *f = fopen("gf_inverse.bin", "wb");
    if (!f) {
        perror("Failed to open file");
        return 1;
    }
    unsigned char * inverse = malloc(256);
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++) {
            if (gf_multiply((unsigned char)i,(unsigned char)j) == 1) {
                inverse[i] = j;
            }
        }
    }
    fwrite(inverse, sizeof(unsigned char), 256, f);
    fclose(f);
}