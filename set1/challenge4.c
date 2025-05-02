#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../common/utils.h"

int main() {
    FILE *file = fopen("4.txt", "r");

    char line[256];
    float worstscore = 0.0;
    char * worstline = NULL; //We will find the line with the worst cipher score, ie it has been jumbled the most

    while (fgets(line, sizeof(line), file)) {
        float currscore = cipherscore(line);
        if (currscore > worstscore) {
            if (worstline) free(worstline);
            int len = strlen(line) + 1;
            worstscore = currscore;
            worstline = malloc(len);
            memcpy(worstline, line, len);
        }
    }

    //Now we decode using challenge 3

    unencodeXORHex(worstline);
}