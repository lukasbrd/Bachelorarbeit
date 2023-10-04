#ifndef BERKELEY
#define BERKELEY

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
#include <db.h>

void writeOneStateToBerkeleyDB(char *state, size_t len, char digest[HASH_LEN]);
char *restoreOneStateFromBerkeleyDB(char digest[HASH_LEN], size_t oldLen);
void deleteOneStateFromBerkeleyDB(char digest[HASH_LEN]);
void restoreAllStatesFromBerkeleyDBToQueue(zsock_t *command, Queue *q);
void initBerkeleyDB();
void closeBerkeleyDB();

#endif