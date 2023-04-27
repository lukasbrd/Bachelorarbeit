#include "hash.h"
#include "queue.h"
#include <stdio.h>
#include <string.h>

// Jeweils Hexadezimalzeichen produzieren ein Byte
// daher müssen 41 Byte reserviert werden
int writeToBuffer(tCell *cell) {
    FILE *fp;

    char readablehash[READABLE_HASH_LEN];
    print_readable_digest(cell->digest, readablehash);

    char path[48] = ""; 

    char directory[] = "buffer/";
    strcpy(path, directory);
    strcat(path, readablehash);
    printf("Path to File: %s\n\n", path);

    fp = fopen(path, "w+");

    if (fp == NULL) {
        perror("Error in opening file");
        return -1;
    }

    fputs(cell->term, fp);
    fputs("\n", fp);

    fputs(cell->digest, fp);   //valgrind Fehler an dieser Stelle
    fputs("\n", fp);
    
    char tmp2[33] = "";
    sprintf(tmp2, "%ld",cell->term_length);
    fputs(tmp2,fp);

    fclose(fp);
    return 0;
}

int deleteFromBuffer(tCell *cell) {
    char readablehash[READABLE_HASH_LEN];
    print_readable_digest(cell->digest, readablehash);

    char path[48] = "";

    char directory[] = "buffer/";
    strcpy(path, directory);
    strcat(path, readablehash);

    remove(path);
}
