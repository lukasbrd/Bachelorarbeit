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

wQueue *init_queue();
tCell *init_cell(char *term);

void sendAndPersist(zsock_t *commandSocket, char *term, int cmd, wQueue *const q);
tCell *receiveAndRestore(zsock_t *command, zsock_t *packageSocket, wQueue *const q);
tCell *dequeue(wQueue *const q);
void enqueue(wQueue *const q, tCell *cell);

int readAllFromStorageToQueue(zsock_t *command, wQueue *const q);
char *readOneTermFromStorage(const char digest[HASH_LEN]);
int deleteFromStorage(const char digest[HASH_LEN]);
int writeToStorage(char *const term, const size_t len, const char digest[HASH_LEN]);

void printCell(tCell *cell);