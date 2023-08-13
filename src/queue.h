#ifndef QUEUE_HEADER
#define QUEUE_HEADER

#include "hash.h"
#include <czmq.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>

#define ENQUEUE 1
#define RESTORED 2
#define DEQUEUE 3
#define TERMINATE 4

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
} Queue;

void enqueue(Queue *const q, Element *element);
Element *dequeue(Queue *const q);
Queue *initQueue();
Element *createElement(char *state);
void printElement(Element *element);
void printAllStatesOfElements(Queue const *const q);

#endif
