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

void enqueue(void* pushSocket ,char *term) {
    tCell *cell = malloc(sizeof(tCell));
    cell->term = NULL;
    cell->term_length = strlen(term);

    hash(term, cell->term_length, cell->digest);

    // Push the struct to the queue
    zmq_send(pushSocket, cell, sizeof(tCell), 0);
    writeToStorage(term,cell->term_length,cell->digest);
    free(term);
    free(cell);
}

tCell *dequeue(void *pullSocket) {
    // Pull the struct from the queue
    tCell *receivedCell = malloc(sizeof(tCell));
    zmq_recv(pullSocket, receivedCell, sizeof(tCell), 0);
    receivedCell->term = readOneTermFromStorage(receivedCell->digest);
    return receivedCell;
}

int main(void) {
    pthread_t thread;
    srand(time(NULL));
    void *context = zmq_ctx_new();

    //bind one socket/longer living one
    void *pushSocket = zmq_socket(context, ZMQ_PUSH);
    zmq_bind(pushSocket, "inproc://queue");

    //connect the other socket
    void *pullSocket = zmq_socket(context, ZMQ_PULL);
    zmq_connect(pullSocket, "inproc://queue");

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
    zmq_close(pushSocket);
    zmq_close(pullSocket);
    zmq_ctx_destroy(context);

    return 0;
}
