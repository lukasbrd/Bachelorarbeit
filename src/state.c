#include "state.h"
#include <stdlib.h>
#include <stdio.h>
#include "settings.h"

char *createState(int numberOfCalls) {
    int digits = numberOfDigits(numberOfCalls);
    char *state;
    int addedLength;

    addedLength = (rand() % MAXADDEDLENGTH) + 1;
    int totalLength = digits + addedLength + 1;
    state = (char *) malloc(sizeof(char) * (totalLength));
    sprintf(state, "%d", numberOfCalls);

    for (int k = digits; k < totalLength; k++) {
        state[k] = 'A';
    }
    state[totalLength - 1] = '\0';
    return state;
}

int numberOfDigits(int i) {
    if (i < 10) return 1;
    if (i < 100) return 2;
    if (i < 1000) return 3;
    if (i < 10000) return 4;
    if (i < 100000) return 5;
    if (i < 1000000) return 6;
    if (i < 10000000) return 7;
    if (i < 100000000) return 8;
    if (i < 1000000000) return 9;
    return 10;
}