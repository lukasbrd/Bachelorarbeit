#ifndef SQLITEDATABASE
#define SQLITEDATABASE

#include "communication.h"
#include "hash.h"
#include "queue.h"
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void writeOneStateToSQLiteDatabase(char *const state, const size_t len, const char digest[HASH_LEN]);
char *readOneStateFromSQLiteDatabase(const char digest[HASH_LEN]);
void deleteOneStateFromSQLiteDatabse(const char digest[HASH_LEN]);
void readAllStatesFromSQLiteDatabaseToQueue(zsock_t *command, Queue *const q);

#endif