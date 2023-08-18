#include "qthread.h"
#include "persistenceInterface.h"
#include "settings.h"

volatile bool threadRunning = false;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *qthread(void *args) {
    zsock_t *commandSocket = zsock_new_pull("inproc://command");
    zsock_t *packageSocket = zsock_new_push("inproc://package");

    pthread_mutex_lock(&mutex);
    threadRunning = true;
    pthread_cond_broadcast(&condition);
    pthread_mutex_unlock(&mutex);

    Queue *q = (Queue *)args;

    while (1) {
        int cmd = 0;
        Element *element = NULL;
        zsock_recv(commandSocket, "ip", &cmd, &element);

        if(cmd == ENQUEUE) {
            persistOneState(element->state, element->stateLength, element->digest);
            stateInMemoryOrNot(q, element);
            enqueue(q, element);
        } else if (cmd == RESTORED) {
            stateInMemoryOrNot(q, element);
            enqueue(q, element);
        } else if (cmd == DEQUEUE) {
            Element *dequeuedElement = NULL;
            dequeuedElement = dequeue(q);
            restoreStateIfNecessary(q, dequeuedElement);
            zsock_send(packageSocket, "p", dequeuedElement);
        } else if (cmd == TERMINATE) {
            zsock_destroy(&commandSocket);
            zsock_destroy(&packageSocket);
            pthread_exit(0);
        } else {
            fprintf(stderr, "Unknown command given to qthread.\n");
        }
    }
}

void restoreStateIfNecessary(Queue *q, Element *dequeuedElement) {
    if (dequeuedElement->state == NULL) {
        dequeuedElement->state = restoreOneState(dequeuedElement->digest, dequeuedElement->stateLength);
        q->not_in_mem--;
    } else {
        q->in_mem--;
    }
}

void stateInMemoryOrNot(Queue *q, Element *element) {
    if (q->in_mem >= MAXINMEMORY) {
        free(element->state);
        element->state = NULL;
        q->not_in_mem++;
    } else {
        q->in_mem++;
    }
}
