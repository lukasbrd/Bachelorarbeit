#include "hash.h"
#include "queue.h"
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int writeToStorage(wQueue *const q, char *const term, const size_t len, const char digest[HASH_LEN]) {
    char readableHash[READABLE_HASH_LEN];
    print_readable_digest(digest, readableHash);

    char dir[48] = "";
    char lastPath[48] = "";
    char firstElementPath[48] = "";
    char folder[] = "storage/";
    strcpy(dir, folder);
    strcat(dir, readableHash);

    int fd = open(dir, O_CREAT, 00700);

    if (fd == -1) {
        perror("Error in opening file");
        return -1;
    }
    char *buf = (char *)malloc(sizeof(char) * (20 + 8 + len));
    // printf("1 digest:%s\n", digest);
    // printf("2 len:%ld\n", len);
    // printf("3 term:%s\n\n", term);

    char charlen[8];
    sprintf(charlen, "%ld", len);

    strcpy(buf, digest);
    strcat(buf, charlen);
    strcat(buf, term);
    // printf("buf: %s",buf);

    write(fd, &buf, strlen(buf));
    free(buf);

    close(fd);
    return 0;
}