#include "queue.h"

Queue *initQueue() {
    Queue *q = (Queue *)malloc(sizeof(Queue));
    q->first = NULL;
    q->last = NULL;
    q->qLength = 0;
    q->in_mem = 0;
    q->not_in_mem = 0;
    return q;
}

Element *createElement(char *state) {
    Element *element = malloc(sizeof(Element));
    element->state = state;
    element->stateLength = strlen(state);
    hash(state, (int) element->stateLength, element->digest);
    element->next = NULL;
    return element;
}

void enqueue(Queue *const q, Element *element) {
    if (q->first == NULL) {
        q->first = element;
        q->last = element;
    } else {
        q->last->next = element;
        q->last = element;
    }
}

Element *dequeue(Queue *const q) {
    Element *res = q->first;
    q->first = q->first->next;
    if (q->first == NULL) {
        q->last = NULL;
    }
    res->next = NULL;
    return res;
}

void printAllStatesOfElements(Queue const *const q) {
    Element *tmp = q->first;
    while (tmp != NULL) {
        printf("Length: %ld\n", tmp->stateLength);
        printf("Text: %s\n", tmp->state);
        tmp = tmp->next;
    }
}

void printElement(Element *element) {
    char digestmain[HASH_LEN + 1];
    printf("State: %s\n", element->state);
    //printf("StateLength: %ld\n", element->stateLength);
    memcpy(digestmain, element->digest, HASH_LEN);
    digestmain[HASH_LEN] = '\0';
    //printf("digest:%s\n", digestmain);
}