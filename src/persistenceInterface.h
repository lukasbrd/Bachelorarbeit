#ifndef PERSISTENCEINTERFACE_HEADER
#define PERSISTENCEINTERFACE_HEADER

#include "queue.h"
#include "hash.h"
#include "queue.h"
#include "communication.h"
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void persistOneTerm(char *const term, const size_t len, const char digest[HASH_LEN]);
char *loadOneTerm(const char digest[HASH_LEN]);
void readAllTerms(zsock_t *command, wQueue *const q);
void deleteOneTerm(const char digest[HASH_LEN]);

#endif
