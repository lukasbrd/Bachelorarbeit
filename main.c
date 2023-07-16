#include "queue.h"
#include "stdio.h"
#include <czmq.h>
#include "randomService.h"

typedef struct Cell {
    char *term;
    size_t term_length;
    char digest[HASH_LEN+1];
} tCell;







int main(void) {
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


    tCell* cell = malloc(sizeof(tCell));
    cell->term = NULL;
    cell->term_length = strlen(term);

    hash(term,cell->term_length, cell->digest);
    cell->digest[HASH_LEN] = '\0';

    // Push the struct to the queue
    zmq_send(pushSocket, cell, sizeof(tCell), 0);


    // Pull the struct from the queue
    tCell* receivedCell = malloc(sizeof(tCell));
    zmq_recv(pullSocket, receivedCell, sizeof(tCell), 0);

    // Print the values of the received struct
    printf("Received struct:\n");
    printf("Term Length: %ld\n", receivedCell->term_length);
    printf("Term : %s\n", receivedCell->term);
    printf("Term digest: %s\n", receivedCell->digest);

    // Cleanup
    free(term);
    free(cell);
    free(receivedCell);
    zmq_close(pushSocket);
    zmq_close(pullSocket);
    zmq_ctx_destroy(context);

    return 0;
}
