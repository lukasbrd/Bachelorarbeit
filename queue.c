#include "queue.h"
#include "hash.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>

wQueue *init_queue() {
    wQueue *q = (wQueue *)malloc(sizeof(wQueue));
    q->first = NULL;
    q->last = NULL;
    q->c = 0;
    q->first_not_in_mem = NULL;
    return q;
}

void *enqueue(void *arg) {
    QueueData *queueData = (QueueData *)arg;
    wQueue *q = queueData->q;
    char *term = queueData->queueTerm;
    size_t len = queueData->len;

    tCell *new = (tCell *)malloc(sizeof(tCell));
    q->c++;
    if (q->c < 2) {
        new->term = strdup(queueData->queueTerm);
    } else {
        new->term = NULL;
    }
    new->term_length = len;
    new->next = NULL;

    memcpy(new->digest, queueData->digest, HASH_LEN);

    if (q->first == NULL) {
        q->first = new;
        q->last = new;
    } else {
        q->last->next = new;
        q->last = new;
    }

    if (q->c == 2) {
        q->first_not_in_mem = new;
    }

    printf("Length: %ld\n", new->term_length);
    printf("Term: %s\n", new->term);

    free(queueData->queueTerm);
    free(queueData);
    pthread_exit(NULL);
}

tCell *dequeue(wQueue *const q) {
    if (q->c == 0) {
        return NULL;
    }
    q->c--;

    tCell *res = q->first;
    q->first = q->first->next;

    if (q->first == NULL) {
        q->last = NULL;
    }
    res->next = NULL;
    return res;
}

void teardown_queue(wQueue *q) {
    tCell *res = NULL;
    while (q->c != 0) {
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
    return q->c;
}

void printAllTermsOfCells(wQueue const *const q) {
    tCell *tmp = q->first;

    while (tmp != NULL) {
        printf("Length: %ld\n", tmp->term_length);
        printf("Text: %s\n", tmp->term);
        tmp = tmp->next;
    }
}