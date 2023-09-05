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

    pthread_t thread;
    pthread_create(&thread, NULL, qthread, (void *)q);

    pthread_mutex_lock(&mutex);
    while (!threadRunning) {
        pthread_cond_wait(&condition, &mutex);
    }
    pthread_mutex_unlock(&mutex);

    //restoreAllStatesToQueue(commandSocket, q);

    int a = NUMBEROFSTATES;
    int c = 1;
    do {
        if (a > 0) {
            for (int b = 1; b <= a; b++) {
                char *state = createState(b);
                //printf("StateStart:%s\n", state);
                sendElement(commandSocket, state, ENQUEUE, q);
            }
        }
        for(int d = 1; d <= c; d++) {
            Element *receivedElement;
            receivedElement = receiveElement(commandSocket, packageSocket, q);
            deleteOneStateFromPersistentStorage(receivedElement);
            //printf("State: %s\n", receivedElement->state);
            free(receivedElement->state);
            free(receivedElement);
        }
        a = a-1;
        c = c+1;
        printf("queueLength:%d\n\n",q->qLength);
    } while(q->qLength > 0);

    zsock_send(commandSocket, "ip", TERMINATE, NULL);
    pthread_join(thread, NULL);
    zsock_destroy(&commandSocket);
    zsock_destroy(&packageSocket);
    free(q);
    return 0;
}
