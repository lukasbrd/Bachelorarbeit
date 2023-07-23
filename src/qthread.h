#ifndef QTHREAD_HEADER
#define QTHREAD_HEADER

#include "queue.h"

extern volatile bool threadrunning;
extern pthread_cond_t condition;
extern pthread_mutex_t mutex;

void *qthread(void *args);

#endif 
