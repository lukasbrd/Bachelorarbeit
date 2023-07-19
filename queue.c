#include "queue.h"
#include "hash.h"
#include <stdio.h>
#include <string.h>

wQueue *init_queue() {
    wQueue *q = (wQueue *)malloc(sizeof(wQueue));
    q->first = NULL;
    q->last = NULL;
    q->qlength = 0;
    return q;
}

void enqueueMem(wQueue *const q, tCell *cell) {
    if (q->first == NULL) {
        q->first = cell;
        q->last = cell;
        cell->next = NULL;
    } else {
        q->last->next = cell;
        q->last = cell;
        cell->next = NULL;
    }
}

tCell *dequeueMem(wQueue *const q) {
    if(q->first == NULL) {
        return NULL;
    }
    tCell *res = q->first;
    q->first = q->first->next;

    if (q->first == NULL) {
        q->last = NULL;
    }
    res->next = NULL;
    return res;
}

int is_empty(wQueue const *const q) {
    return (q->first == NULL);
}


void printAllTermsOfCells(wQueue const *const q) {
    tCell *tmp = q->first;

    while (tmp != NULL) {
        printf("Length: %ld\n", tmp->term_length);
        printf("Text: %s\n", tmp->term);
        tmp = tmp->next;
    }
}