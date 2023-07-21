#include "queue.h"
#include <stdio.h>
#include <string.h>

wQueue *init_queue() {
    wQueue *q = (wQueue *)malloc(sizeof(wQueue));
    q->first = NULL;
    q->last = NULL;
    q->qlength = 0;
    return q;
}

tCell *init_cell(char *term) {
    tCell *cell = malloc(sizeof(tCell));
    cell->term = term;
    cell->term_length = strlen(term);
    hash(term, cell->term_length, cell->digest);
    cell->next = NULL;
    return cell;
}

void enqueue(wQueue *const q, tCell *cell) {
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

tCell *dequeue(wQueue *const q) {
    tCell *res = q->first;
    q->first = q->first->next;
    if (q->first == NULL) {
        q->last = NULL;
    }
    res->next = NULL;
    return res;
}

void printAllTermsOfCells(wQueue const *const q) {
    tCell *tmp = q->first;
    while (tmp != NULL) {
        printf("Length: %ld\n", tmp->term_length);
        printf("Text: %s\n", tmp->term);
        tmp = tmp->next;
    }
}

void printCell(tCell *cell) {
    char digestmain[HASH_LEN + 1];
    printf("receivedTerm: %s\n", cell->term);
    printf("receivedTermLength: %ld\n", cell->term_length);
    memcpy(digestmain, cell->digest, HASH_LEN);
    digestmain[HASH_LEN] = '\0';
    printf("digest:%s\n", digestmain);
}