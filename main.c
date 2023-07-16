#include "queue.h"
#include "stdio.h"
#include <czmq.h>

typedef struct Cell {
    char *term;
    size_t term_length;
    char digest[HASH_LEN];
} tCell;

pthread_t thread;













int main(void) {
    void *context = zmq_ctx_new();

    //bind one socket/longer living one
    void *pushSocket = zmq_socket(context, ZMQ_PUSH);
    zmq_bind(pushSocket, "inproc://queue");

    //connect the other socket
    void *pullSocket = zmq_socket(context, ZMQ_PULL);
    zmq_connect(pullSocket, "inproc://queue");

    tCell* cell = malloc(sizeof(tCell));
    cell->term = strdup("Example Term");
    cell->term_length = strlen(cell->term)+1;

    hash(cell->term,cell->term_length, cell->digest);

    // Push the struct to the queue
    zmq_send(pushSocket, &cell, sizeof(tCell), 0);

    // Pull the struct from the queue
    tCell receivedCell;
    zmq_recv(pullSocket, &receivedCell, sizeof(tCell), 0);

    // Print the values of the received struct
    printf("Received struct:\n");
    printf("Term: %s\n", receivedCell.term);
    printf("Term Length: %zu\n", receivedCell.term_length);
    printf("Term digest: %s\n", receivedCell.digest);

    // Cleanup
    free(cell->term);
    free(cell);
    zmq_close(pushSocket);
    zmq_close(pullSocket);
    zmq_ctx_destroy(context);

    return 0;
}
