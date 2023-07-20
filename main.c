#include "queue.h"
#include "randomService.h"
#include "stdio.h"
#include <czmq.h>

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

void sendAndPersist(zsock_t *commandSocket, char *term, int cmd, wQueue *q) {
    tCell *cell = init_cell(term);
    if (cmd == ENQUEUE) {
        writeToStorage(cell->term, cell->term_length, cell->digest);
    }
    if (q->qlength > INMEMORY) {
        cell->term = NULL;
        free(term);
    }
    zsock_send(commandSocket, "ip", cmd, cell);
    (q->qlength)++;
    printf("EnqueueLength: %d\n", q->qlength);
}

tCell *receiveAndRestore(zsock_t *command, zsock_t *packageSocket, wQueue *q) {
    if (q->qlength == 0) {
        return NULL;
    }
    tCell *receivedCell = NULL;
    zsock_send(command, "ip", DEQUEUE, NULL);
    int rc = zsock_recv(packageSocket, "p", &receivedCell);
    assert(rc == 0);
    if (receivedCell->term == NULL) {
        receivedCell->term = readOneTermFromStorage(receivedCell->digest);
    }
    (q->qlength)--;
    printf("DequeueLength: %d\n", q->qlength);
    return receivedCell;
}

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

    readAllFromStorageToQueue(commandSocket, q);

    if (q->qlength > 0) {
        tCell *receivedCell = NULL;
        receivedCell = receiveAndRestore(commandSocket, packageSocket, q);
        printCell(receivedCell);
        free(receivedCell->term);
        free(receivedCell);
    }

    for (int i = 0; i < 2; i++) {
        char *term = createRandomString();
        printf("TermStart:%s\n", term);
        sendAndPersist(commandSocket, term, ENQUEUE, q);
    }

    while (q->qlength > 0) {
        tCell *receivedCell = NULL;
        receivedCell = receiveAndRestore(commandSocket, packageSocket, q);
        printCell(receivedCell);
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
