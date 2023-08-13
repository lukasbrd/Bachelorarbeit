#include "fileHandler.h"

void writeOneStateToFileStorage(char *const state, const size_t len, const char digest[HASH_LEN]) {
    char readableHash[READABLE_HASH_LEN];
    print_readable_digest(digest, readableHash);
    int fd;

    mkdir("storage", 0700);

    char dir[41] = "storage/";
    strcat(dir, readableHash);

    if ((fd = open(dir, O_WRONLY | O_CREAT | O_TRUNC, 00600)) == -1) {
        fprintf(stderr, "Error opening file1: '%s': %s\n", dir, strerror(errno));
        return;
    }

    char buf[28 + len];

    memcpy(buf, digest, 20);
    memcpy(buf + 20, &len, sizeof(size_t));
    memcpy(buf + 28, state, len);

    write(fd, buf, (28 + len));
    close(fd);
}

char *readOneStateFromFileStorage(const char digest[HASH_LEN]) {
    char readableHash[READABLE_HASH_LEN];
    print_readable_digest(digest, readableHash);
    int fd;
    size_t len;
    char buf[28];
    char *state;

    mkdir("storage", 0700);

    char dir[41] = "storage/";
    strcat(dir, readableHash);

    if ((fd = open(dir, O_RDONLY, 00600)) == -1) {
        fprintf(stderr, "Error opening file2: '%s': %s\n", dir, strerror(errno));
        return NULL;
    }

    read(fd, buf, 28);
    memcpy(&len, buf + 20, sizeof(size_t));
    state = (char *)malloc(sizeof(char) * (len + 1));
    read(fd, state, len);
    state[len] = '\0';
    close(fd);

    if (memcmp(buf, digest, HASH_LEN) != 0) {
        fprintf(stderr, "The fileDigest does not match the given digest.\n ");
    }

    printf("readOneFromFileStorage: %s\n", state);
    return state;
}

void deleteOneStateFromFileStorage(const char digest[HASH_LEN]) {
    char readableHash[READABLE_HASH_LEN];
    print_readable_digest(digest, readableHash);

    mkdir("storage", 0700);

    char dir[41] = "storage/";
    strcat(dir, readableHash);

    int del = remove(dir);
    if (del == 0) {
        return;
    } else {
        fprintf(stderr, "Error deleting file: %s\n", strerror(errno));
    }
}

void readAllStatesFromFileStorageToQueue(zsock_t *command, Queue *const q) {
    size_t len;
    char digest[HASH_LEN];
    char buf[28];
    int fd;

    DIR *dir;
    struct dirent *dp;

    mkdir("storage", 0700);

    dir = opendir("storage/");
    if (dir != NULL) {
        while ((dp = readdir(dir))) {
            if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
                fd = openat(dirfd(dir), dp->d_name, O_RDONLY);
                read(fd, buf, 28);
                memcpy(digest, buf, 20);
                memcpy(&len, buf + 20, sizeof(size_t));
                char *state = (char *)malloc(sizeof(char) * (len + 1));
                state[len] = '\0';
                read(fd, state, len);
                sendAndPersist(command, state, RESTORED, q);
                close(fd);
            }
        }
    } else {
        fprintf(stderr, "Error opening directory: %s\n", strerror(errno));
    }
    closedir(dir);
}
