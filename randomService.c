#include "randomService.h"
#include <stdio.h>
#include <stdlib.h>

char *createRandomString(char *term) {
    int stringLength;
    stringLength = (rand() % 50) + 1;
    term = (char *) malloc(sizeof(char) * (stringLength+1));

    int i;
    for (i = 0; i < stringLength; i++) {
        term[i] = (char)(rand() % ('Z' - 'A')) + 'A';
    }
    term[i] = '\0';
    return term;
}

