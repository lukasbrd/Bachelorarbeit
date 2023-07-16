#include "queue.h"
#include "stdio.h"
#include <czmq.h>
#include "randomService.h"

#define ENQUEUE 1
#define DEQUEUE 2

typedef struct Cell {
    char *term;
    size_t term_length;
    char digest[HASH_LEN];
} tCell;

void enqueue(zsock_t *pushSocket ,char *term) {
    tCell *cell = malloc(sizeof(tCell));
    cell->term = NULL;
    cell->term_length = strlen(term);

    hash(term, cell->term_length, cell->digest);

    // Push the struct to the queue
    zsock_send(pushSocket,"ip",1, cell);
    writeToStorage(term,cell->term_length,cell->digest);
    free(term);
}

tCell *dequeue(zsock_t *pullSocket) {
    // Pull the struct from the queue
    tCell *receivedCell;
    int cmd;
    zsock_recv(pullSocket,"ip",&cmd,&receivedCell);
    receivedCell->term = readOneTermFromStorage(receivedCell->digest);
    return receivedCell;
}

int main(void) {
    pthread_t thread;
    srand(time(NULL));

    //bind one socket/longer living one
    zsock_t *pushSocket = zsock_new_push("inproc://queue");

    //connect the other socket
    zsock_t *pullSocket = zsock_new_pull("inproc://queue");

    char *term = createRandomString();
    printf("TermStart:%s\n",term);

    enqueue(pushSocket,term);

    tCell *receivedCell;
    receivedCell = dequeue(pullSocket);

    // Print the values of the received struct
    printf("Received struct:\n");
    printf("receivedTerm Length: %ld\n", receivedCell->term_length);
    printf("receivedTerm : %s\n", receivedCell->term);

    // Cleanup
    free(receivedCell->term);
    free(receivedCell);
    zsock_destroy(&pushSocket);
    zsock_destroy(&pullSocket);

    return 0;
}
