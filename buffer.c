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
    printf("Currentpath to File: %s\n\n", currentPath);
    fprintf(fp, "%zu %s ", len, term);
    fclose(fp);

    if (q->in_mem == 0) {
        memcpy(q->not_in_mem_first, currentReadableHash, READABLE_HASH_LEN);
        memcpy(q->not_in_mem_last, currentReadableHash, READABLE_HASH_LEN);

        strcpy(firstElementPath, directory);
        strcat(firstElementPath, "start");

        fp = fopen(firstElementPath, "w");

        if (fp == NULL) {
            perror("Error in opening file");
            return -1;
        }
        printf("Firstelementpath to File: %s\n\n", firstElementPath);
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
        printf("Lastpath to File: %s\n\n", firstElementPath);
        memcpy(q->not_in_mem_last, currentReadableHash, READABLE_HASH_LEN);
        fclose(fp);
    }
    fp = NULL;
    q->in_mem++;
    return 0;
}

int deleteFromBuffer(const char digest[HASH_LEN]) {
    char readablehash[READABLE_HASH_LEN];
    print_readable_digest(digest, readablehash);
    printf("DeleteHash: %s\n", readablehash);

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
    char term[len + 1];

    fscanf(fp, "%s %s", term, readhash);
    memcpy(q->not_in_mem_first, readhash, READABLE_HASH_LEN);

    // printf("Length: %zu\n", len);
    // printf("Term: %s\n", term);
    // printf("ReadHash: %s\n\n", readhash);

    fclose(fp);
    fp = NULL;
    hash(term, strlen(term), digest);
    enqueue(q, term, len, digest);
}
