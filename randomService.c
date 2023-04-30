#include "randomService.h"
#include <stdio.h>
#include <stdlib.h>

char *createRandomString(char *term) {
    int stringLength;
    stringLength = (rand() % 50) + 1;

    int i;
    for (i = 0; i < stringLength; i++) {
        term[i] = (char)(rand() % (ASCII_END - ASCII_START)) + ASCII_START;
    }
    term[i] = '\0';
    return term;
}

