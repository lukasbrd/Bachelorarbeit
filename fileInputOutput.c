#include "fileInputOutput.h"
#include <stdio.h>

int writeOneField(char *const term, int write) {
    printf("writer: %d\n", write);
    FILE *fp;
    fp = fopen("buffer.txt", "r+b");

    if (fp == NULL) {
        perror("Error in opening file");
        return -1;
    }

    fseek(fp,write*10, SEEK_SET);
    fputs(term,fp);
    fputs("\n",fp);
    fclose(fp);
    return ++write;
}

int writeToFile(char *const term, int read, int write) {
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

/*
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
