#ifndef QTHREAD_HEADER
#define QTHREAD_HEADER

#include "queue.h"

extern volatile bool threadRunning;
extern pthread_cond_t condition;
extern pthread_mutex_t mutex;

void *qthread(void *args);
void stateInMemoryOrNot(Queue *q, Element *element);
void restoreStateIfNecessary(Queue *q, Element *dequeuedElement);

#endif 
