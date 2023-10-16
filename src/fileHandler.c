#include "fileHandler.h"


void writeOneStateToFileStorage(char *state, size_t len, const char digest[HASH_LEN]) {
    char readableHash[READABLE_HASH_LEN];
    print_readable_digest(digest, readableHash);
    int fd;

    mkdir("files", 0700);

    char dir[47] = "files/";
    strcat(dir, readableHash);

    if ((fd = open(dir, O_WRONLY | O_CREAT | O_EXCL, 00600)) == -1) {
        return;
    }
    char buf[28 + len];

    memcpy(buf, digest, 20);
    memcpy(buf + 20, &len, sizeof(size_t));
    memcpy(buf + 28, state, len);

    write(fd, buf, (28 + len));
    close(fd);
}

char *restoreOneStateFromFileStorage(const char digest[HASH_LEN], size_t oldLen) {
    char readableHash[READABLE_HASH_LEN];
    char fileDigest[HASH_LEN];
    char newDigest[HASH_LEN];
    print_readable_digest(digest, readableHash);
    int fd;
    size_t len;
    char buf[28];
    char *state;

    mkdir("files", 0700);

    char dir[47] = "files/";
    strcat(dir, readableHash);

    if ((fd = open(dir, O_RDONLY, 00600)) == -1) {
        return NULL;
    }

    read(fd, buf, 28);
    memcpy(fileDigest,buf,HASH_LEN);
    memcpy(&len, buf + 20, sizeof(size_t));

    if (len != oldLen) {
        fprintf(stderr, "A saved stateLength is different from the restored stateLength.\n");
        return NULL;
    }

    state = (char *)malloc(sizeof(char) * (len + 1));
    read(fd, state, len);
    state[len] = '\0';
    close(fd);

    hash(state, (int) len, newDigest);
    if (memcmp(fileDigest, newDigest , HASH_LEN) != 0) {
        fprintf(stderr, "A State was corrupted.\n ");
        free(state);
        return NULL;
    }
    return state;
}

void deleteOneStateFromFileStorage(const char digest[HASH_LEN]) {
    char readableHash[READABLE_HASH_LEN];
    print_readable_digest(digest, readableHash);

    mkdir("files", 0700);

    char dir[47] = "files/";
    strcat(dir, readableHash);
    remove(dir);
}

void restoreAllStatesFromFileStorageToQueue(zsock_t *command, Queue *const q) {
    size_t len;
    char fileDigest[HASH_LEN];
    char newDigest[HASH_LEN];
    char buf[28];
    int fd;

    DIR *dir;
    struct dirent *dp;

    mkdir("files", 0700);

    dir = opendir("files/");
    if (dir != NULL) {
        while ((dp = readdir(dir))) {
            if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
                fd = openat(dirfd(dir), dp->d_name, O_RDONLY);
                off_t endOfFile = lseek(fd, 0, SEEK_END);
                if(endOfFile < 29) {
                    close(fd);
                    continue;
                } else {
                    lseek(fd,0, SEEK_SET);
                }
                read(fd, buf, 28);
                memcpy(fileDigest, buf, 20);
                memcpy(&len, buf + 20, sizeof(size_t));
                char *state = (char *)malloc(sizeof(char) * (len + 1));
                state[len] = '\0';
                read(fd, state, len);
                hash(state, (int) len, newDigest);
                if (memcmp(fileDigest, newDigest , HASH_LEN) != 0) {
                    fprintf(stderr, "A State was corrupted.\n ");
                    free(state);
                } else {
                    printf("Restored: %s\n", state);
                    enqueueElementWithState(command, state, RESTORED, q);
                }
                close(fd);
            }
        }
    } else {
        fprintf(stderr, "Error opening directory: %s\n", strerror(errno));
    }
    closedir(dir);
}
