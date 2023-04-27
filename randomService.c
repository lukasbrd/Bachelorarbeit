#include "randomService.h"
#include <stdio.h>
#include <stdlib.h>


char *createRandomString() {
    int stringLength;
    stringLength = 1 + rand() % 50;

    int i;
    char *randomString = malloc(stringLength+1);

    for (i = 0; i < stringLength; i++) {
        randomString[i] = (char)(rand() % (ASCII_END - ASCII_START)) + ASCII_START;
    }
    randomString[i] = '\0';
    return randomString;
}