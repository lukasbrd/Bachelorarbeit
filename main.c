#include "queue.h"
#include "randomService.h"
#include "stdio.h"
#include <czmq.h>

volatile bool threadrunning = false;

void *threaddi(void *args) {
    zsock_t *commandSocket = zsock_new_pull("inproc://command");
    zsock_t *packageSocket = zsock_new_push("inproc://package");

    threadrunning = true;
    wQueue *q = (wQueue *)args;

    while (1) {
        int cmd = 0;
        tCell *cell = NULL;
        int rc = zsock_recv(commandSocket, "ip", &cmd, &cell);
        assert(rc == 0);

        if (cmd == ENQUEUE || cmd == READFROMSTORAGE) {
            enqueueMem(q, cell);
        } else if (cmd == DEQUEUE) {
            tCell *receivedCell = NULL;
            receivedCell = dequeueMem(q);
            zsock_send(packageSocket, "p", receivedCell);
        } else if (cmd == TERMINATE) {
            zsock_destroy(&commandSocket);
            zsock_destroy(&packageSocket);
            pthread_exit(0);
        }
    }
}

void enqueue(zsock_t *commandSocket, char *term, int cmd, wQueue *q) {
    tCell *cell = malloc(sizeof(tCell));
    cell->term = term;
    cell->term_length = strlen(term);
    hash(term, cell->term_length, cell->digest);
    cell->next = NULL;
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

tCell *dequeue(zsock_t *command, zsock_t *packageSocket, wQueue *q) {
    if(q->qlength == 0) {
        return NULL;
    }
    tCell *receivedCell = NULL;
    zsock_send(command, "ip", DEQUEUE, NULL);
    int rc = zsock_recv(packageSocket, "p", &receivedCell);
    if (rc == 0 && receivedCell->term == NULL) {
        receivedCell->term = readOneTermFromStorage(receivedCell->digest);
    }
    (q->qlength)--;
    printf("DequeueLength: %d\n", q->qlength);
    return receivedCell;
}

int main(void) {
    pthread_t thread;
    srand(time(NULL));
    wQueue *q = init_queue();

    zsock_t *commandSocket = zsock_new_push("inproc://command");
    zsock_t *packageSocket = zsock_new_pull("inproc://package");

    pthread_create(&thread, NULL, threaddi, (void *)q);
    while (!threadrunning) {
    }

    readAllFromStorageToQueue(commandSocket, q);

    char digestmain2[HASH_LEN + 1];
    if(q->qlength >0) {
        tCell *receivedCell = NULL;
        receivedCell = dequeue(commandSocket, packageSocket, q);
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
        enqueue(commandSocket, term, ENQUEUE, q);
    }

    char digestmain[HASH_LEN + 1];
    while (q->qlength > 0) {
        tCell *receivedCell = NULL;
        receivedCell = dequeue(commandSocket, packageSocket, q);
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
    free(q);
    return 0;
}
