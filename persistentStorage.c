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
    int fd;

    char dir[41] = "storage/"; // 41 = 8 + 32 + 1
    strcat(dir, readableHash);

    if ((fd = open(dir, O_RDWR | O_CREAT | O_TRUNC, 00600)) == -1) {
        perror("Cannot open output file\n");
        exit(1);
    }

    // printf("digest: %s\n",digest);
    // printf("len: %ld\n",len);
    // printf("term: %s\n",term);

    char buf[28 + len];

    memcpy(buf, digest, 20);
    memcpy(buf + 20, &len, sizeof(size_t));
    memcpy(buf + 28, term, len);

    write(fd, buf, (28 + len));

    /*
    size_t len2 = 0;
    char digest2[HASH_LEN] = {0};

    read(fd, buf, 28);

    memcpy(&len2, buf + 20, sizeof(size_t));
    memcpy(digest2, buf, 20);

    printf("len2: %ld\n",len2);
    printf("digest2: %s\n",digest2);*/

    return 0;
}