#include "queue.h"
#include "randomService.h"
#include "stdio.h"
#include <czmq.h>



#define ENQUEUE 1
#define DEQUEUE 2
#define TERMINATE 3
#define INMEMORY 2

typedef struct Cell {
    char *term;
    size_t term_length;
    char digest[HASH_LEN];
} tCell;

void *threaddi(void *args) {
    zsock_t *commandSocket = zsock_new_pull("inproc://command");
    zsock_t *packageSocket = zsock_new_push("inproc://package");

    zsock_t *enqueue = zsock_new_push("inproc://queue");
    zsock_t *dequeue = zsock_new_pull("inproc://queue");

    int queueLength = 0;

    while (1) {
        int cmd;
        tCell *cell;
        int rc = zsock_recv(commandSocket, "ip", &cmd, &cell);
        assert(rc == 0);

        if (cmd == ENQUEUE) {
            writeToStorage(cell->term, cell->term_length, cell->digest);
            if (queueLength > INMEMORY) {
                free(cell->term);
                cell->term = NULL;
            }
            zsock_send(enqueue, "p", cell);
            queueLength++;
            printf("queueLength1:%d\n",queueLength);
        } else if (cmd == DEQUEUE) {
            tCell *receivedCell;
            zsock_recv(dequeue, "p", &receivedCell);
            if (receivedCell->term == NULL) {
                receivedCell->term = readOneTermFromStorage(receivedCell->digest);
            }
            zsock_send(packageSocket, "p", receivedCell);
            queueLength--;
            printf("queueLength2:%d\n", queueLength);
        } else if (cmd == TERMINATE) {
            zsock_destroy(&commandSocket);
            zsock_destroy(&packageSocket);
            zsock_destroy(&enqueue);
            zsock_destroy(&dequeue);
            pthread_exit(NULL);
        }
    }
}

void enqueue(zsock_t *commandSocket, char *term) {
    tCell *cell = malloc(sizeof(tCell));
    cell->term = term;
    cell->term_length = strlen(term);
    hash(term, cell->term_length, cell->digest);
    zsock_send(commandSocket, "ip", ENQUEUE, cell);
}

tCell *dequeue(zsock_t *command, zsock_t *packageSocket) {
    tCell *receivedCell;
    zsock_send(command, "ip", DEQUEUE, NULL);
    zsock_recv(packageSocket, "p", &receivedCell);
    return receivedCell;
}

int main(void) {
    pthread_t thread;
    srand(time(NULL));
    char digestmain[HASH_LEN + 1];

    pthread_create(&thread, NULL, threaddi, NULL);
    sleep(3);

    zsock_t *commandSocket = zsock_new_push("inproc://command");
    zsock_t *packageSocket = zsock_new_pull("inproc://package");

    for (int i = 0; i < 4; i++) {
        char *term = createRandomString();
        printf("TermStart:%s\n", term);

        enqueue(commandSocket, term);
    }

    for(int k = 0; k < 4; k++) {
        tCell *receivedCell;
        receivedCell = dequeue(commandSocket, packageSocket);

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
