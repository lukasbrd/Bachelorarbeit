#include "communication.h"
#include "persistenceInterface.h"
#include "qthread.h"
#include "queue.h"
#include "state.h"
#include <czmq.h>
#include <stdio.h>
#include "settings.h"

int main(void) {
    zsock_t *commandSocket = zsock_new_push("inproc://command");
    zsock_t *packageSocket = zsock_new_pull("inproc://package");
    Queue *q = initQueue();
    pthread_t thread;
    pthread_create(&thread, NULL, qthread, (void *)q);
    pthread_mutex_lock(&mutex);
    while (!threadRunning) {
        pthread_cond_wait(&condition, &mutex);
    }
    pthread_mutex_unlock(&mutex);
    restoreAllStates(commandSocket, q);

    srand(1);
    for (int i = 1; i <= NUMBEROFSTATES; i++) {
        char *state = createState(i);
        printf("StateStart:%s\n", state);
        sendAndPersist(commandSocket, state, ENQUEUE, q);
    }
    while (q->qLength > 0) {
        Element *receivedElement = NULL;
        receivedElement = receiveAndRestore(commandSocket, packageSocket, q);
        printf("State: %s\n", receivedElement->state);
        deleteOneState(receivedElement->digest);
        free(receivedElement->state);
        free(receivedElement);
    }
    zsock_send(commandSocket, "ip", TERMINATE, NULL);
    pthread_join(thread, NULL);
    zsock_destroy(&commandSocket);
    zsock_destroy(&packageSocket);
    free(q);
    return 0;
}
