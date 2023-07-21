#ifndef threadHeader
#define threadHeader

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

#define INMEMORY 6

typedef struct Cell {
    char *term;
    char digest[HASH_LEN];
    size_t term_length;
    struct Cell *next;
} tCell;

typedef struct workQueue {
    tCell *first;
    tCell *last;
    int qlength;
} wQueue;

void enqueue(wQueue *const q, tCell *cell);
tCell *dequeue(wQueue *const q);
wQueue *init_queue();
tCell *init_cell(char *term);
void printCell(tCell *cell);
void printAllTermsOfCells(wQueue const *const q);

#endif