#include "hash.h"
#include "queue.h"
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int writeToStorage(wQueue *const q, char *const term, const size_t len, char digest[HASH_LEN]) {
    char readableHash[READABLE_HASH_LEN];
    print_readable_digest(digest, readableHash);
    int fd;

    char dir[41] = "storage/";
    strcat(dir, readableHash);

    if ((fd = open(dir, O_RDWR | O_CREAT | O_TRUNC, 00600)) == -1) {
        perror("Cannot open output file\n");
        exit(1);
    }

    char buf[28 + len];

    memcpy(buf, digest, 20);
    memcpy(buf + 20, &len, sizeof(size_t));
    memcpy(buf + 28, term, len);

    write(fd, buf, (28 + len));
    close(fd);
    return 0;
}

int deleteFromStorage(const char digest[HASH_LEN]) {
    char readableHash[READABLE_HASH_LEN];
    print_readable_digest(digest, readableHash);

    char dir[41] = "storage/";
    strcat(dir, readableHash);

    int del = remove(dir);
    return 0;
}

int readOneTermFromStorageToQueue(wQueue *q) {
    char readableHash[READABLE_HASH_LEN];
    print_readable_digest(q->first_not_in_mem->digest, readableHash);
    int fd;
    size_t len;
    char buf[28];
    char *term;

    char dir[41] = "storage/";
    strcat(dir, readableHash);

    if ((fd = open(dir, O_RDWR, 00600)) == -1) {
        perror("Cannot open output file\n");
        exit(1);
    }

    read(fd, buf, 28);
    memcpy(&len, buf + 20, sizeof(size_t));
    term = (char *)malloc(sizeof(char) * (len + 1));
    read(fd,term,len);
    term[len] = '\0';
    printf("OneTermToQueue:%s\n", term);
    q->first_not_in_mem->term = term;
    if (q->first_not_in_mem->next != NULL) {
        q->first_not_in_mem = q->first_not_in_mem->next;
    }
    close(fd);
    return 0;
}

int readAllFromStorageToQueue(wQueue *const q) {
    size_t len;
    char digest[HASH_LEN];
    char buf[28];
    int fd;

    DIR *dir;
    struct dirent *dp;

    dir = opendir("storage/");
    if (dir != NULL) {
        while (dp = readdir(dir)) {
            if (strcmp(dp->d_name, ".") && strcmp(dp->d_name, "..")) {
                fd = openat(dirfd(dir), dp->d_name, O_RDONLY);
                read(fd, buf, 28);
                memcpy(digest, buf, 20);
                memcpy(&len, buf + 20, sizeof(size_t));
                char *term = (char *)malloc(sizeof(char) * (len + 1));
                term[len] = '\0';
                read(fd, term, len);
                enqueue(q, term, len, digest);
                close(fd);
            }
        }
    } else {
        perror("Couldn't open the directory");
    }
    closedir(dir);
    return 0;
}