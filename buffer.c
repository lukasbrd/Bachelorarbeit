#include "hash.h"
#include "queue.h"
#include <stdio.h>
#include <string.h>

int writeToBuffer(wQueue *const q, char *const term, const size_t len, const char digest[HASH_LEN]) {
    FILE *fp;
    char currentReadableHash[READABLE_HASH_LEN];
    char lastReadableHash[READABLE_HASH_LEN];
    print_readable_digest(digest, currentReadableHash);

    char currentPath[48] = "";
    char lastPath[48] = "";
    char firstElementPath[48] = "";
    char directory[] = "buffer/";
    strcpy(currentPath, directory);
    strcat(currentPath, currentReadableHash);

    fp = fopen(currentPath, "w");

    if (fp == NULL) {
        perror("Error in opening file");
        return -1;
    }
    fprintf(fp, "%zu %s ", len, term);
    fclose(fp);

    if (q->in_mem == 0) {
        memcpy(q->not_in_mem_last, currentReadableHash, READABLE_HASH_LEN);

        strcpy(firstElementPath, directory);
        strcat(firstElementPath, "start");

        fp = fopen(firstElementPath, "w");

        if (fp == NULL) {
            perror("Error in opening file");
            return -1;
        }
        fprintf(fp, "%s", currentReadableHash);
        fclose(fp);
    } else {
        strcpy(lastPath, directory);
        memcpy(lastReadableHash, q->not_in_mem_last, READABLE_HASH_LEN);
        strcat(lastPath, lastReadableHash);

        fp = fopen(lastPath, "a");

        if (fp == NULL) {
            perror("Error in opening file");
            return -1;
        }
        fprintf(fp, "%s", currentReadableHash);
        memcpy(q->not_in_mem_last, currentReadableHash, READABLE_HASH_LEN);
        fclose(fp);
    }
    fp = NULL;
    q->in_mem++;
    if(q->in_mem > 2 && q->not_in_mem_first[0] == '\0') {
        memcpy(q->not_in_mem_first, currentReadableHash, READABLE_HASH_LEN);
    }
    return 0;
}

int deleteFromBuffer(wQueue *const q,const char digest[HASH_LEN]) {
    FILE *fp;
    char oldFirstReadablehash[READABLE_HASH_LEN];
    char newFirstReadablehash[READABLE_HASH_LEN];
    print_readable_digest(digest, oldFirstReadablehash);

    char path[48] = "";
    char directory[] = "buffer/";
    strcpy(path, directory);
    strcat(path, oldFirstReadablehash);

    fp = fopen(path, "r");
    if (fp == NULL) {
        perror("Error in opening file");
        return -1;
    }
    size_t len = 0;
    fscanf(fp, "%zu", &len);
    char term[len];
    fscanf(fp, "%s %s",term,newFirstReadablehash);
    memcpy(q->not_in_mem_first,newFirstReadablehash,READABLE_HASH_LEN);
    fclose(fp);

    fp = fopen("buffer/start", "w");
    fprintf(fp, "%s", newFirstReadablehash);
    fclose(fp);
    fp = NULL;
    remove(path);
}

int readFromBufferToQueue(wQueue *const q) {
    FILE *fp;
    char oldFirstreadablehash[READABLE_HASH_LEN];
    memcpy(oldFirstreadablehash, q->not_in_mem_first, READABLE_HASH_LEN);
    char path[48] = "";
    char directory[] = "buffer/";
    strcpy(path, directory);
    strcat(path, oldFirstreadablehash);

    fp = fopen(path, "r");

    if (fp == NULL) {
        perror("Error in opening file");
        return -1;
    }

    size_t len = 0;
    char newFirstreadablehash[READABLE_HASH_LEN] = "";
    char digest[HASH_LEN] = "";

    fscanf(fp, "%zu", &len);
    fclose(fp);
    char term[len];

    fp = fopen(path, "r");
    fscanf(fp, "%zu %s %s",&len, term, newFirstreadablehash);
    memcpy(q->not_in_mem_first, newFirstreadablehash, READABLE_HASH_LEN);
    printf("Len1: %ld\n", len);
    printf("Term1: %s\n", term);
    printf("Hash1: %s\n", newFirstreadablehash);

    fclose(fp);
    fp = NULL;
    hash(term, strlen(term), digest);
    enqueue(q, term, len, digest);
    q->in_mem--;
    return 0;
}
