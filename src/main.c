#include "communication.h"
#include "persistenceInterface.h"
#include "qthread.h"
#include "queue.h"
#include "state.h"
#include <czmq.h>
#include <stdio.h>
#include "settings.h"
#include <time.h>

int main(void) {
    zsock_t *commandSocket = zsock_new_push("inproc://command");
    zsock_t *packageSocket = zsock_new_pull("inproc://package");
    Queue *q = initQueue();
    struct timeval start_time, end_time;
    double elapsed_time;


    pthread_t thread;
    pthread_create(&thread, NULL, qthread, (void *)q);

    pthread_mutex_lock(&mutex);
    while (!threadRunning) {
        pthread_cond_wait(&condition, &mutex);
    }
    pthread_mutex_unlock(&mutex);

    restoreAllStatesToQueue(commandSocket, q);


    gettimeofday(&start_time, NULL);
    srand(0);
    for (int i = 1; i <= NUMBEROFSTATES; i++) {
        char *state = createState(i);
        printf("StateStart:%s\n", state);
        sendElement(commandSocket, state, ENQUEUE, q);
    }
    gettimeofday(&end_time, NULL);

    elapsed_time = (end_time.tv_sec - start_time.tv_sec) +
                   (end_time.tv_usec - start_time.tv_usec) / 1e6;


    printf("Time taken to execute in seconds : %f\n",elapsed_time);
    while (q->qLength > 0) {
        Element *receivedElement;
        receivedElement = receiveElement(commandSocket, packageSocket, q);
        deleteOneStateFromPersistentStorage(receivedElement);
        printf("State: %s\n", receivedElement->state);
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
