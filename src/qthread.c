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

        if (cmd == ENQUEUE || cmd == RESTORED) {
            if(cmd == ENQUEUE) {
                persistOneState(element->state, element->stateLength, element->digest);
            }
            if (q->in_mem >= MAXINMEMORY) {
                free(element->state);
                element->state = NULL;
                q->not_in_mem++;
                printf("insertnotInMem: %d\n",q->not_in_mem);
            } else {
                q->in_mem++;
                printf("insertinMem: %d\n",q->in_mem);
            }
            enqueue(q, element);
        } else if (cmd == DEQUEUE) {
            Element *dequeuedElement = NULL;
            dequeuedElement = dequeue(q);
            if (dequeuedElement->state == NULL) {
                dequeuedElement->state = restoreOneState(dequeuedElement->digest);
                q->not_in_mem--;
                printf("DequeueNotInMem: %d\n",q->not_in_mem);
            } else {
                q->in_mem--;
                printf("DequeueInMem: %d\n",q->in_mem);
            }
            zsock_send(packageSocket, "p", dequeuedElement);
        } else if (cmd == TERMINATE) {
            zsock_destroy(&commandSocket);
            zsock_destroy(&packageSocket);
            pthread_exit(0);
        }
    }
}