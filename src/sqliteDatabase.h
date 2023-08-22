#ifndef SQLITEDATABASE
#define SQLITEDATABASE

#include "communication.h"
#include "qthread.h"
#include "hash.h"
#include "queue.h"
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void writeOneStateToSQLiteDatabase(char *state, size_t len, const char digest[HASH_LEN]);
char *restoreOneStateFromSQLiteDatabase(const char digest[HASH_LEN], size_t oldLen);
void deleteOneStateFromSQLiteDatabse(const char digest[HASH_LEN]);
void restoreAllStatesFromSQLiteDatabaseToQueue(zsock_t *command, Queue *q);

#endif