#ifndef PERSISTENCEINTERFACE_HEADER
#define PERSISTENCEINTERFACE_HEADER

#include "hash.h"
#include "queue.h"
#include "communication.h"


void persistOneState(char *state, size_t len, const char digest[HASH_LEN]);
char *restoreOneState(const char digest[HASH_LEN], size_t oldLen);
void restoreAllStatesToQueue(zsock_t *command, Queue *const q);
void deleteOneState(const char digest[HASH_LEN]);

#endif
