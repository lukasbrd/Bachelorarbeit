#include "hash.h"
#include <czmq.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct Cell {
    char *term;
    size_t term_length;
    char digest[HASH_LEN];  
} tCell;


void enqueue(zsock_t *zock, char *term, wQueue *const q);
tCell *dequeue(zsock_t *cmd_send, zsock_t *package_recv, wQueue *const q);
char *readOneTermFromStorage(const char digest[HASH_LEN]);
int deleteFromStorage(const char digest[HASH_LEN]);
int writeToStorage(char *const term, const size_t len, const char digest[HASH_LEN]);