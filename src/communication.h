#ifndef COMMUNICATION_HEADER
#define COMMUNICATION_HEADER

#include "queue.h"
#include "hash.h"
#include <czmq.h>

void sendAndPersist(zsock_t *commandSocket, char *state, int cmd, Queue *q);
Element *receiveAndRestore(zsock_t *command, zsock_t *packageSocket, Queue *q);
#endif