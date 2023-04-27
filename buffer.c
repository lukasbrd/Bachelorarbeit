#include "hash.h"
#include "queue.h"
#include <stdio.h>
#include <string.h>

// Jeweils 2 Hexadezimalzeichen produzieren ein Byte
// daher müssen 41 Byte reserviert werden
/*int writeToBuffer(tCell *cell) {
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
    free(cell->term);


    fputs(cell->digest, fp);   //valgrind Fehler
    fputs("\n", fp);
    
    char tmp2[33] = "";
    sprintf(tmp2, "%ld", cell->term_length);
    fputs(tmp2, fp);

    fclose(fp);
    return 0;
}*/


int writeOnlyToBuffer(char *const term, const size_t len,const char digest[HASH_LEN]) {
    FILE *fp;

    char readablehash[READABLE_HASH_LEN];
    print_readable_digest(digest, readablehash);

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

    fputs(term, fp);
    fputs("\n", fp);
    
    for(int i=0; i < HASH_LEN; i++) {
        fputc(digest[i],fp);
    }
    fputs("\n", fp);

    
    char tmp[8] = "";
    sprintf(tmp, "%ld", len);
    fputs(tmp, fp);

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
