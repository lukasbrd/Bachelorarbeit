#include "queue.h"
#include "randomService.h"
#include "stdio.h"
#include <czmq.h>

volatile bool threadrunning = false;

void *threaddi(void *args) {
    zsock_t *commandSocket = zsock_new_pull("inproc://command");
    zsock_t *packageSocket = zsock_new_push("inproc://package");

    threadrunning = true;
    wQueue *q = init_queue();

    while (1) {
        int cmd = 0;
        tCell *cell = NULL;
        int rc = zsock_recv(commandSocket, "ip", &cmd, &cell);
        assert(rc == 0);

        if (cmd == ENQUEUE || cmd == READFROMSTORAGE) {
            enqueueMem(q,cell);
        } else if (cmd == DEQUEUE) {
            tCell *receivedCell = NULL;
            receivedCell = dequeueMem(q);
            zsock_send(packageSocket, "p", receivedCell);
        } else if (cmd == TERMINATE) {
            zsock_destroy(&commandSocket);
            zsock_destroy(&packageSocket);
            free(q);
            pthread_exit(0);
        }
    }
}

void enqueue(zsock_t *commandSocket, char *term, int cmd, int *queueLength) {
    tCell *cell = malloc(sizeof(tCell));

    cell->term = term;
    cell->term_length = strlen(term);
    hash(term, cell->term_length, cell->digest);
    cell->next = NULL;

    if (cmd == ENQUEUE) {
        writeToStorage(cell->term, cell->term_length, cell->digest);
    }
    if (*queueLength > INMEMORY) {
        cell->term = NULL;
        free(term);
    }
    zsock_send(commandSocket, "ip", cmd, cell);
    (*queueLength)++;
    printf("EnqueueLength: %d\n",*queueLength);
}

tCell *dequeue(zsock_t *command, zsock_t *packageSocket, int *queueLength) {
    tCell *receivedCell = NULL;
    zsock_send(command, "ip", DEQUEUE, NULL);
    int rc = zsock_recv(packageSocket, "p", &receivedCell);
    if (rc == 0 && receivedCell->term == NULL) {
        receivedCell->term = readOneTermFromStorage(receivedCell->digest);
    }
    (*queueLength)--;
    printf("DequeueLength: %d\n", *queueLength);
    return receivedCell;
}

int main(void) {
    pthread_t thread;
    srand(time(NULL));
    int queueLength = 0;

    zsock_t *commandSocket = zsock_new_push("inproc://command");
    zsock_t *packageSocket = zsock_new_pull("inproc://package");

    pthread_create(&thread, NULL, threaddi, NULL);
    while (!threadrunning) {
    }

    readAllFromStorageToQueue(commandSocket, &queueLength);

    char digestmain2[HASH_LEN + 1];
    for(int i=0; i < 3; i++) {
        tCell *receivedCell = NULL;
        receivedCell = dequeue(commandSocket, packageSocket, &queueLength);
        printf("receivedTerm: %s\n", receivedCell->term);
        printf("receivedTermLength: %ld\n", receivedCell->term_length);
        memcpy(digestmain2, receivedCell->digest, HASH_LEN);
        digestmain2[HASH_LEN] = '\0';
        printf("digest:%s\n", digestmain2);
        free(receivedCell->term);
        free(receivedCell);
    }

    for (int i = 0; i < 2; i++) {
        char *term = createRandomString();
        printf("TermStart:%s\n", term);
        enqueue(commandSocket, term, ENQUEUE, &queueLength);
    }

    char digestmain[HASH_LEN + 1];
    
    while (queueLength > 0) {
        tCell *receivedCell = NULL;
        receivedCell = dequeue(commandSocket, packageSocket, &queueLength);
        printf("receivedTerm: %s\n", receivedCell->term);
        printf("receivedTermLength: %ld\n", receivedCell->term_length);
        memcpy(digestmain, receivedCell->digest, HASH_LEN);
        digestmain[HASH_LEN] = '\0';
        printf("digest:%s\n", digestmain);
        free(receivedCell->term);
        free(receivedCell);
    }

    zsock_send(commandSocket, "ip", TERMINATE, NULL);
    pthread_join(thread, NULL);

    zsock_destroy(&commandSocket);
    zsock_destroy(&packageSocket);
    return 0;
}
