#ifndef QUEUE_HEADER
#define QUEUE_HEADER

#include "hash.h"
#include <czmq.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct Element {
    char *state;
    char digest[HASH_LEN];
    size_t stateLength;
    struct Element *next;
} Element;

typedef struct Queue {
    Element *first;
    Element *last;
    int qLength;
    int in_mem;
    int not_in_mem;
} Queue;

void enqueue(Queue *q, Element *element);
Element *dequeue(Queue *q);
Queue *initQueue();
Element *createElement(char *state);
char *getState(Element *receivedElement);
void printElement(Element *element);
void printAllStatesOfElements(Queue const *q);

#endif
