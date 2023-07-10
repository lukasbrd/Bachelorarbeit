#include "queue.h"
#include "hash.h"
#include <stdio.h>
#include <string.h>

wQueue *init_queue() {
    wQueue *q = (wQueue *)malloc(sizeof(wQueue));
    q->first = NULL;
    q->last = NULL;
    q->in_mem = ATOMIC_VAR_INIT(0);
    q->not_in_mem = ATOMIC_VAR_INIT(0);
    q->first_not_in_mem = NULL;
    return q;
}

void enqueue(wQueue *const q, char *const term, const size_t len, char digest[HASH_LEN]) {
    bool firstTermInMem = true;
    tCell *new = (tCell *)malloc(sizeof(tCell));
    if (atomic_load(&q->in_mem) < 3) {
        new->term = term;
        atomic_fetch_add(&q->in_mem, 1);
    } else {
        free(term);
        new->term = NULL;
        atomic_fetch_add(&q->not_in_mem, 1);
        if(firstTermInMem) {
            q->first_not_in_mem = new;
            firstTermInMem = false;
        }
    }
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

    printf("Length: %ld\n", new->term_length);
    printf("Term: %s\n", new->term);
}

tCell *dequeue(wQueue *const q) {
    if (atomic_load(&q->in_mem) == 0) {
        return NULL;
    }
    atomic_fetch_sub(&q->in_mem, 1);

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
    while ((atomic_load(&q->in_mem) + atomic_load(&q->not_in_mem)) > 0) {
        if (atomic_load(&q->not_in_mem) > 0) {
            readOneTermFromStorageToQueue(q);
            atomic_fetch_sub(&q->not_in_mem, 1);
            atomic_fetch_add(&q->in_mem, 1);
        }
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
    return q->in_mem +q->not_in_mem;
    }

    void printAllTermsOfCells(wQueue const *const q) {
    tCell *tmp = q->first;

    while (tmp != NULL) {
        printf("Length: %ld\n", tmp->term_length);
        printf("Text: %s\n", tmp->term);
        tmp = tmp->next;
    }
    }