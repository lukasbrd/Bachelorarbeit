#include "queue.h"
#include "randomService.h"
#include "stdio.h"
#include <czmq.h>

#define ENQUEUE 1
#define DEQUEUE 2
#define TERMINATE 3

bool runner = true;

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

    while (runner) {
        int cmd;
        tCell *cell;
        int rc = zsock_recv(commandSocket, "ip", &cmd, &cell);
        assert(rc == 0);

        if (cmd == ENQUEUE) {
            zsock_send(enqueue, "p", cell);
        } else if (cmd == DEQUEUE) {
            tCell *receivedCell;
            zsock_recv(dequeue, "p", &receivedCell);
            zsock_send(packageSocket, "p", receivedCell);
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
    cell->term = NULL;
    cell->term_length = strlen(term);

    hash(term, cell->term_length, cell->digest);

    zsock_send(commandSocket, "ip", ENQUEUE, cell);
    writeToStorage(term, cell->term_length, cell->digest);
    free(term);
}

tCell *dequeue(zsock_t *command, zsock_t *packageSocket) {
    tCell *receivedCell;
    zsock_send(command, "ip", DEQUEUE, NULL);
    zsock_recv(packageSocket, "p", &receivedCell);

    receivedCell->term = readOneTermFromStorage(receivedCell->digest);
    return receivedCell;
}

int main(void) {
    pthread_t thread;
    srand(time(NULL));

    pthread_create(&thread, NULL, threaddi, NULL);
    sleep(3);

    zsock_t *commandSocket = zsock_new_push("inproc://command");
    zsock_t *packageSocket = zsock_new_pull("inproc://package");

    char *term = createRandomString();
    printf("TermStart:%s\n", term);

    enqueue(commandSocket, term);

    tCell *receivedCell;
    receivedCell = dequeue(commandSocket, packageSocket);

    printf("Received struct:\n");
    printf("receivedTerm Length: %ld\n", receivedCell->term_length);
    printf("receivedTerm : %s\n", receivedCell->term);

    free(receivedCell->term);
    free(receivedCell);

    runner = false;

    zsock_send(commandSocket, "ip", TERMINATE, NULL);

    pthread_join(thread, NULL);
    zsock_destroy(&commandSocket);
    zsock_destroy(&packageSocket);

    return 0;
}
