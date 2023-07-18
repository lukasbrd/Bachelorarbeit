#include "hash.h"
#include <czmq.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>

#define ENQUEUE 1
#define NOPERSIST 2
#define DEQUEUE 3
#define TERMINATE 4

#define INMEMORY 2



char *readOneTermFromStorage(const char digest[HASH_LEN]);
int deleteFromStorage(const char digest[HASH_LEN]);
int writeToStorage(char *const term, const size_t len, const char digest[HASH_LEN]);
int readAllFromStorageToQueue(zsock_t *command);
void enqueue(zsock_t *commandSocket, char *term, int cmd);