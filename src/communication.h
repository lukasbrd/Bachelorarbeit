#ifndef COMMUNICATION_HEADER
#define COMMUNICATION_HEADER

#include "queue.h"
#include "qthread.h"
#include "hash.h"
#include <czmq.h>

void enqueueElementWithState(zsock_t *commandSocket, char *state, int cmd, Queue *q);
Element *dequeueElementWithState(zsock_t *commandSocket, zsock_t *packageSocket, Queue *q);
#endif