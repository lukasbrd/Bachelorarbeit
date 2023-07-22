#include "communication.h"
#include "persistentStorage.h"
#include "qthread.h"
#include "queue.h"
#include "randomService.h"
#include <czmq.h>
#include <stdio.h>

int main(void) {
    srand(time(NULL));
    zsock_t *commandSocket = zsock_new_push("inproc://command");
    zsock_t *packageSocket = zsock_new_pull("inproc://package");

    wQueue *q = init_queue();

    pthread_t thread;
    pthread_create(&thread, NULL, qthread, (void *)q);
    pthread_mutex_lock(&mutex);
    while (!threadrunning) {
        pthread_cond_wait(&condition, &mutex);
    }
    pthread_mutex_unlock(&mutex);

    readAllTermsFromStorageToQueue(commandSocket, q);

    if (q->qlength > 0) {
        tCell *receivedCell = NULL;
        receivedCell = receiveAndRestore(commandSocket, packageSocket, q);
        printCell(receivedCell);
        deleteOneTermFromStorage(receivedCell->digest);
        free(receivedCell->term);
        free(receivedCell);
    }

    for (int i = 0; i < 4; i++) {
        char *term = createRandomString();
        printf("TermStart:%s\n", term);
        sendAndPersist(commandSocket, term, ENQUEUE, q);
    }

    while (q->qlength > 0) {
        tCell *receivedCell = NULL;
        receivedCell = receiveAndRestore(commandSocket, packageSocket, q);
        printCell(receivedCell);
        deleteOneTermFromStorage(receivedCell->digest);
        free(receivedCell->term);
        free(receivedCell);
    }

    zsock_send(commandSocket, "ip", TERMINATE, NULL);
    pthread_join(thread, NULL);

    zsock_destroy(&commandSocket);
    zsock_destroy(&packageSocket);
    free(q);
    return 0;
}
