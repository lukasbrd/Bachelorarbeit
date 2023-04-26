#include <stdio.h>

#include "hash.h"

void hash(char *input, int len, char *digest) {
    sha1(input, len, digest);
}

void print_readable_digest(unsigned char const * const key, char *dest) {
    int i;
    for (i = 0; i < HASH_LEN; i++) {
        sprintf(dest + 2*i, "%02x", key[i]);
    }
}

