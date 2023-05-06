#include "hash.h"
#include "queue.h"
#include <stdio.h>
#include <string.h>

int writeToBuffer(wQueue *const q, char *const term, const size_t len, const char digest[HASH_LEN]) {
    FILE *fp;

    char readablehash[READABLE_HASH_LEN];
    print_readable_digest(digest, readablehash);
    if (q->not_in_mem_first != "") {
        memcpy(q->not_in_mem_first, readablehash, READABLE_HASH_LEN);
    }
    printf("Hier1: %s\n\n", q->not_in_mem_first);

    char path[48] = "";

    char directory[] = "buffer/";
    strcpy(path, directory);
    strcat(path, readablehash);
    printf("Writingpath to File: %s\n\n", path);

    fp = fopen(path, "w+");

    if (fp == NULL) {
        perror("Error in opening file");
        return -1;
    }
    fprintf(fp, "%zu %s %s", len, term, readablehash);
    fclose(fp);
    fp = NULL;
    return 0;
}

int deleteFromBuffer(const char digest[HASH_LEN]) {
    char readablehash[READABLE_HASH_LEN];
    print_readable_digest(digest, readablehash);

    char path[48] = "";

    char directory[] = "buffer/";
    strcpy(path, directory);
    strcat(path, readablehash);

    remove(path);
}

int readFromBufferToQueue(wQueue *const q) {
    FILE *fp;
    char readablehash[READABLE_HASH_LEN];
    memcpy(readablehash, q->not_in_mem_first, READABLE_HASH_LEN);
    char path[48] = "";
    char directory[] = "buffer/";
    strcpy(path, directory);
    strcat(path, readablehash);
    printf("Readingpath to File: %s\n\n", path);

    fp = fopen(path, "r");

    if (fp == NULL) {
        perror("Error in opening file");
        return -1;
    }

    size_t len = 0;
    char readhash[READABLE_HASH_LEN] = "";
    char digest[HASH_LEN] = "";
    
    fscanf(fp, "%zu", &len);
    char term[len+1];

    fscanf(fp,"%s %s", term, readhash);

    printf("Length: %zu\n", len);
    printf("Term: %s\n", term);
    printf("ReadHash: %s\n\n", readhash);

    fclose(fp);
    fp = NULL;
    hash(term,strlen(term),digest);
    enqueue(q, term, len, digest);
}
