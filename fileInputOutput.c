#include "fileInputOutput.h"
#include <stdio.h>


int writeToFile(char *const term, int read, int write) {
    FILE *fp;
    fp = fopen("buffer.txt", "a");
    printf("write: %d\n", write);

    if (fp == NULL) {
        perror("Error in opening file");
        return -1;
    }

    // Der Schreibkopf muss zuerst gestartet werden, da er zuerst etwas in den Buffer schreiben muss, bevor es ausgelesen werden kann.

    // Wenn der Schreibkopf ein Feld hinter dem Lesekopf ist soll nicht mehr geschrieben werden. Erst wenn das nächste Feld frei ist, wird wieder geschrieben
    if (write == read - 1 || (write == 9999 && read == 0)) {
        return write;
        // Am Ende des Ringbuffers soll der Schreibkopf erst schreiben und dann an den Anfang springen
    } else if (write == 9999 && read != 0) {
        fputs(term, fp);
        fputs("\n", fp);
        fclose(fp);
        write = 0;
        return write;
        // Wenn das nächste Feld frei ist soll erst geschrieben und dann ein Feld weitergegangen werden
        // Wenn beide auf demselben Feld sind, muss der Lesevorgang abgeschlossen sein, bevor geschrieben werden darf. Das kann hier nicht koodiniert werden
    } else {
        fputs(term, fp);
        fputs("\n", fp);
        fclose(fp);
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

    // Wenn der Lesekopf auf demselben Feld ist, wie der Schreibkopf darf nur einmal gelesen werden

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
