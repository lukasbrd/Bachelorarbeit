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

void writeOneTermToSQLiteDatabase(char *const term, const size_t len, const char digest[HASH_LEN]);
char *readOneTermFromSQLiteDatabase(const char digest[HASH_LEN]);
void deleteOneTermFromSQLiteDatabse(const char digest[HASH_LEN]);
void readAllTermsFromSQLiteDatabase(zsock_t *command, wQueue *const q);

#endif