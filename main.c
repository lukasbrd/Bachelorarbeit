#include "queue.h"
#include "randomService.h"
#include "stdio.h"
#include <czmq.h>

typedef struct Cell {
    char *term;
    size_t term_length;
    char digest[HASH_LEN];
} tCell;

bool threadrunning = false;

void *threaddi(void *args) {
    zsock_t *commandSocket = zsock_new_pull("inproc://command");
    zsock_t *packageSocket = zsock_new_push("inproc://package");
    zsock_t *enqueue = zsock_new_push("inproc://queue");
    zsock_t *dequeue = zsock_new_pull("inproc://queue");

    threadrunning = true;

    int queueLength = 0;

    while (1) {
        int cmd = 0;
        tCell *cell = NULL;
        int rc = zsock_recv(commandSocket, "ip", &cmd, &cell);
        assert(rc == 0);

        if (cmd == ENQUEUE || cmd == NOPERSIST) {
            queueLength++;
            if (cmd == ENQUEUE) {
                writeToStorage(cell->term, cell->term_length, cell->digest);
            }
            if (queueLength > INMEMORY) {
                free(cell->term);
                cell->term = NULL;
            }
            zsock_send(enqueue, "p", cell);
            printf("queueLength1:%d\n", queueLength);
        } else if (cmd == DEQUEUE) {
            queueLength--;
            tCell *receivedCell = NULL;
            zsock_set_rcvtimeo(dequeue, 200);
            int rc = zsock_recv(dequeue, "p", &receivedCell);
            if (rc == 0 && receivedCell->term == NULL) {
                receivedCell->term = readOneTermFromStorage(receivedCell->digest);
            }
            zsock_send(packageSocket, "p", receivedCell);
            printf("queueLength2:%d\n", queueLength);
        } else if (cmd == TERMINATE) {
            zsock_destroy(&commandSocket);
            zsock_destroy(&packageSocket);
            zsock_destroy(&enqueue);
            zsock_destroy(&dequeue);
            pthread_exit(0);
        }
    }
}

void enqueue(zsock_t *commandSocket, char *term, int cmd) {
    tCell *cell = malloc(sizeof(tCell));
    cell->term = term;
    cell->term_length = strlen(term);
    hash(term, cell->term_length, cell->digest);
    zsock_send(commandSocket, "ip", cmd, cell);
}

tCell *dequeue(zsock_t *command, zsock_t *packageSocket) {
    tCell *receivedCell = NULL;
    zsock_send(command, "ip", DEQUEUE, NULL);
    zsock_set_rcvtimeo(packageSocket, 200);
    int rc = zsock_recv(packageSocket, "p", &receivedCell);
    return receivedCell;
}

int main(void) {
    pthread_t thread;
    srand(time(NULL));
    char digestmain[HASH_LEN + 1];
    zsock_t *commandSocket = zsock_new_push("inproc://command");
    zsock_t *packageSocket = zsock_new_pull("inproc://package");

    pthread_create(&thread, NULL, threaddi, NULL);
    while (!threadrunning) {
    }

    readAllFromStorageToQueue(commandSocket);

    for (int i = 0; i < 4; i++) {
        char *term = createRandomString();
        printf("TermStart:%s\n", term);

        enqueue(commandSocket, term, ENQUEUE);
    }

    tCell *receivedCell = NULL;
    while (1) {
        receivedCell = dequeue(commandSocket, packageSocket);
        if (receivedCell == NULL) {
            break;
        }
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
