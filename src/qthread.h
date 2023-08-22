#ifndef QTHREAD_HEADER
#define QTHREAD_HEADER

#include "queue.h"

extern volatile bool threadRunning;
extern pthread_cond_t condition;
extern pthread_mutex_t mutex;

#define ENQUEUE 1
#define RESTORED 2
#define DEQUEUE 3
#define TERMINATE 4

_Noreturn void *qthread(void *args);
void deleteStateIfMemFull(Queue *q, Element *element);
void restoreStateIfItWasDeleted(Queue *q, Element *dequeuedElement);

#endif 
