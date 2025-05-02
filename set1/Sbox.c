#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../common/utils.h"

int main() {
    unsigned char sbox[256];
    sbox[0] = 0x63;
    unsigned char inverse_sbox[256];
    inverse_sbox[0] = 0x52;
    unsigned char inverse[256];
    FILE *newfile = fopen("sbox.bin", "wb");
    FILE *oldfile = fopen("gf_inverse.bin", "rb");
    fread(inverse, sizeof(unsigned char), 256, oldfile);

    for (int i = 0; i < 256; i++) {
        sbox[i] = affine_transform(inverse[i]);
        inverse_sbox[sbox[i]] = i;
    }
    fwrite(sbox, sizeof(unsigned char), 256, newfile);
    fwrite(inverse_sbox, sizeof(unsigned char), 256, newfile);
    fclose(newfile);
    fclose(oldfile);
}