#ifndef COMMUNICATION_HEADER
#define COMMUNICATION_HEADER

#include "queue.h"
#include "hash.h"
#include <czmq.h>

void sendAndPersist(zsock_t *commandSocket, char *term, int cmd, wQueue *q);
tCell *receiveAndRestore(zsock_t *command, zsock_t *packageSocket, wQueue *q);
#endif