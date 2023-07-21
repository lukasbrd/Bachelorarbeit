#ifndef PERSISTENTSTORAGE
#define PERSISTENTSTORAGE

#include "hash.h"
#include "queue.h"
#include <assert.h>
#include <czmq.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int writeOneTermToStorage(char *const term, const size_t len, const char digest[HASH_LEN]);
char *readOneTermFromStorage(const char digest[HASH_LEN]);

int deleteOneTermFromStorage(const char digest[HASH_LEN]);
int readAllTermsFromStorageToQueue(zsock_t *command, wQueue *const q);

#endif
