#ifndef PERSISTENCEINTERFACE_HEADER
#define PERSISTENCEINTERFACE_HEADER

#include "hash.h"
#include "queue.h"
#include "communication.h"
void persistOneState(Element *element);
char *restoreOneState(Element *element);
void restoreAllStatesToQueue(zsock_t *command, Queue *q);
void deleteOneStateFromPersistentStorage(Element *element);
#endif
