#ifndef FILEHANDLER_HEADER
#define FILEHANDLER_HEADER

#include "hash.h"
#include "queue.h"
#include "communication.h"
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void writeOneStateToFileStorage(char *state, size_t len, const char digest[HASH_LEN]);
char *readOneStateFromFileStorage(const char digest[HASH_LEN], size_t oldLen);
void deleteOneStateFromFileStorage(const char digest[HASH_LEN]);
void restoreAllStatesFromFileStorageToQueue(zsock_t *command, Queue *q);

#endif