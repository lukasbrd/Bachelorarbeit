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

    wQueue *q = (wQueue *)args;

    while (1) {
        int cmd = 0;
        tCell *cell = NULL;
        int rc = zsock_recv(commandSocket, "ip", &cmd, &cell);
        assert(rc == 0);

        if (cmd == ENQUEUE || cmd == RESTORED) {
            enqueue(q, cell);
        } else if (cmd == DEQUEUE) {
            tCell *receivedCell = NULL;
            receivedCell = dequeue(q);
            zsock_send(packageSocket, "p", receivedCell);
        } else if (cmd == TERMINATE) {
            zsock_destroy(&commandSocket);
            zsock_destroy(&packageSocket);
            pthread_exit(0);
        }
    }
}