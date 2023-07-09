#include "queue.h"
#include "hash.h"
#include <stdio.h>
#include <string.h>

wQueue *init_queue() {
    wQueue *q = (wQueue *)malloc(sizeof(wQueue));
    q->first = NULL;
    q->last = NULL;
    q->count_in_mem = 0;
    q->count_not_in_mem = 0;
    return q;
}

void enqueue(wQueue *const q, char *const term, const size_t len, char digest[HASH_LEN]) {
    tCell *new = (tCell *)malloc(sizeof(tCell));
    if(q->count_in_mem <= 2) {
        new->in_memory = true;
        q->count_in_mem++;
    } else if(q->count_in_mem > 2) {
        new->in_memory = false;
        q->count_not_in_mem++;
    }
    new->term=term;
    new->term_length = len;
    new->next = NULL;

    memcpy(new->digest, digest, HASH_LEN);

    if (q->first == NULL) {
        q->first = new;
        q->last = new;
    } else {
        q->last->next = new;
        q->last = new;
    }

    //printf("Length: %ld\n", new->term_length);
    //printf("Term: %s\n", new->term);
}



tCell *dequeue(wQueue *const q) {
    if (q->count_in_mem == 0 && q->count_not_in_mem == 0) {
        return NULL;
    }
    if(q->first->in_memory == true) {
        q->count_in_mem--;
    } else if (q->first->in_memory == false) {
        q->count_not_in_mem--;
    }

    tCell *res = q->first;
    q->first = q->first->next;

    if (q->first == NULL) {
        q->last = NULL;
    }
    res->next = NULL;
    return res;
    }

    void teardown_queue(wQueue * q) {
    tCell *res = NULL;
    while (q->count_in_mem != 0 || q->count_not_in_mem != 0) {
        res = dequeue(q);
        free(res->term);
        free(res);
    }
    free(q);
    }

    int is_empty(wQueue const *const q) {
    return (q->first == NULL);
    }

    
    size_t q_size(wQueue const *const q) {
    return q->count_in_mem + q->count_not_in_mem;
    }

    void printAllTermsOfCells(wQueue const *const q) {
    tCell *tmp = q->first;

    while (tmp != NULL) {
        printf("Length: %ld\n", tmp->term_length);
        printf("Text: %s\n", tmp->term);
        tmp = tmp->next;
    }
    }