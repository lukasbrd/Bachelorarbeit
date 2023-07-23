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

int writeOneTermToFileStorage(char *const term, const size_t len, const char digest[HASH_LEN]);
char *readOneTermFromFileStorage(const char digest[HASH_LEN]);
int deleteOneTermFromFileStorage(const char digest[HASH_LEN]);
int readAllTermsFromFileStorageToQueue(zsock_t *command, wQueue *const q);

#endif