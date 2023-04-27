#include "queue.h"
#include <stdio.h>
#include "hash.h"
#include <string.h>

//Jeweils Hexadezimalzeichen produzieren ein Byte 
//daher müssen 41 Byte reserviert werden

int writeToBuffer(tCell *new) {
    FILE *fp;

    char readablehash[READABLE_HASH_LEN];
    print_readable_digest(new->digest, readablehash);

    char path[48];

    char directory[] = "buffer/";
    strcpy(path,directory);
    strcat(path,readablehash);
    printf("Path: %s\n",path);

    fp = fopen(path, "w+");

    if (fp == NULL) {
        perror("Error in opening file");
        return -1;
    }

    fputs(new->term,fp);
    fclose(fp);
    return 0;
}



/* innt writeFile(char *const term, int read, int write) {
    if (write == read - 1 || (write == 9999 && read == 0)) {
        return write;
    } else if (write == 9999 && read != 0) {
        writeOneField(term, write);
        write = 0;
        return write;
    } else {
        writeOneField(term, write);
        write = write + 1;
        return write;
    }
}


int readFromFile(int read, int write) {
    FILE *fp;

    if (fp == NULL) {
        perror("Error in opening file");
        return -1;
    }

    if (read == write) {
        // TODO: Lese aus der Datei aus und gib den term zurück
        // return term;
    }
    // Sonst soll einmal gelesen werden und dann ein Feld vorgegangen werden
    else {
        // TODO: Lese aus der Datei aus und gib den term zurück
        read = read + 1;
        // return term;
    }
    fclose(fp);
} */
