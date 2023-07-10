#include "hash.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>

// Definition einer Zelle
// size_t ist ein unsigned int
typedef struct Cell {
    char *term;
    char digest[HASH_LEN];
    size_t term_length;
    struct Cell *next;
} tCell;

// Definition der Queue mit:
// Pointer auf die erste Zelle
// Pointer auf die Zweite Zelle
// Groesse der Queue als unsigned int
typedef struct workQueue {
    tCell *first;
    tCell *last;
    size_t c;
    tCell *first_not_in_mem;
} wQueue;

typedef struct Data{
    wQueue *q;
    char *term;
} data;


wQueue *init_queue();
void enqueue(wQueue *const q, char *const term, const size_t len, char digest[HASH_LEN]);
tCell *dequeue(wQueue *const q);
void teardown_queue(wQueue *q);
int is_empty(wQueue const *const q);
size_t q_size(wQueue const *const q);
void printAllTermsOfCells(wQueue const *const q);

int writeToStorage(char *const term, const size_t len, char digest[HASH_LEN]);
int deleteFromStorage(const char digest[HASH_LEN]);
int readAllFromStorageToQueue(wQueue *const q);
int readOneTermFromStorageToQueue(wQueue *const q);
void enqueueWithoutTerm(wQueue *const q, char *const term, const size_t len, const char digest[HASH_LEN]);
void *enqueueTerm(void *input);
void *dequeueTerm(void *input);
