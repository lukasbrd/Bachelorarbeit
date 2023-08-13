#include "qthread.h"

volatile bool threadrunning = false;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *qthread(void *args) {
    zsock_t *commandSocket = zsock_new_pull("inproc://command");
    zsock_t *packageSocket = zsock_new_push("inproc://package");

    pthread_mutex_lock(&mutex);
    threadrunning = true;
    pthread_cond_broadcast(&condition);
    pthread_mutex_unlock(&mutex);

    Queue *q = (Queue *)args;

    while (1) {
        int cmd = 0;
        Element *element = NULL;

        zsock_recv(commandSocket, "ip", &cmd, &element);

        if (cmd == ENQUEUE || cmd == RESTORED) {
            enqueue(q, element);
        } else if (cmd == DEQUEUE) {
            Element *dequeuedElement = NULL;
            dequeuedElement = dequeue(q);
            zsock_send(packageSocket, "p", dequeuedElement);
        } else if (cmd == TERMINATE) {
            zsock_destroy(&commandSocket);
            zsock_destroy(&packageSocket);
            pthread_exit(0);
        }
    }
}