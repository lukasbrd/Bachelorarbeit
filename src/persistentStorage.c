#include "persistentStorage.h"

int writeOneTermToStorage(char *const term, const size_t len, const char digest[HASH_LEN]) {
    char readableHash[READABLE_HASH_LEN];
    print_readable_digest(digest, readableHash);
    int fd;

    int mkdirResult = mkdir("storage", 0700);

    char dir[41] = "storage/";
    strcat(dir, readableHash);

    if ((fd = open(dir, O_WRONLY | O_CREAT, 00600)) == -1) {
        fprintf(stderr, "Error opening file1: '%s': %s\n", dir, strerror(errno));
        return 1;
    }

    char buf[28 + len];

    memcpy(buf, digest, 20);
    memcpy(buf + 20, &len, sizeof(size_t));
    memcpy(buf + 28, term, len);

    write(fd, buf, (28 + len));
    close(fd);
    return 0;
}

char *readOneTermFromStorage(const char digest[HASH_LEN]) {
    char readableHash[READABLE_HASH_LEN];
    print_readable_digest(digest, readableHash);
    int fd;
    size_t len;
    char buf[28];
    char *term;

    int mkdirResult = mkdir("storage", 0700);

    char dir[41] = "storage/";
    strcat(dir, readableHash);

    if ((fd = open(dir, O_RDONLY, 00600)) == -1) {
        fprintf(stderr, "Error opening file2: '%s': %s\n", dir, strerror(errno));
    }

    read(fd, buf, 28);
    memcpy(&len, buf + 20, sizeof(size_t));
    term = (char *)malloc(sizeof(char) * (len + 1));
    read(fd, term, len);
    term[len] = '\0';
    close(fd);

    assert(memcmp(buf,digest,20) == 0);
    printf("readOneFromStorage: %s\n", term);
    return term;
}

int deleteOneTermFromStorage(const char digest[HASH_LEN]) {
    char readableHash[READABLE_HASH_LEN];
    print_readable_digest(digest, readableHash);

    int mkdirResult = mkdir("storage", 0700);

    char dir[41] = "storage/";
    strcat(dir, readableHash);

    int del = remove(dir);
    if (del == 0) {
        return 0;
    } else {
        fprintf(stderr, "Error deleting file: %s\n", strerror(errno));
        return 1;                    
    }
}

int readAllTermsFromStorageToQueue(zsock_t *command, wQueue *const q) {
    size_t len;
    char digest[HASH_LEN];
    char buf[28];
    int fd;

    DIR *dir;
    struct dirent *dp;

    int mkdirResult = mkdir("storage", 0700);

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
                sendAndPersist(command, term, RESTORED, q);
                close(fd);
            }
        }
    } else {
        fprintf(stderr, "Error opening directory: %s\n", strerror(errno));
    }
    closedir(dir);
    return 0;
}
