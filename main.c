#include "queue.h"
#include "stdio.h"
#include <czmq.h>

typedef struct Cell {
    char *term;
    size_t term_length;
    char digest[HASH_LEN];
} tCell;

int main(void) {
    // Create a ZMQ context
    void *context = zmq_ctx_new();

    // Create a push socket and bind it to an inproc endpoint
    void *pushSocket = zmq_socket(context, ZMQ_PUSH);
    zmq_bind(pushSocket, "inproc://queue");

    // Create a pull socket and connect it to the inproc endpoint
    void *pullSocket = zmq_socket(context, ZMQ_PULL);
    zmq_connect(pullSocket, "inproc://queue");

    // Create a tCell struct and set its values
    tCell cell;
    cell.term = "Example Term";
    cell.term_length = strlen(cell.term)+1;

    // Set digest to some predefined value
    hash(cell.term,cell.term_length, cell.digest);

    // Push the struct to the queue
    zmq_send(pushSocket, &cell, sizeof(tCell), 0);

    // Pull the struct from the queue
    tCell receivedCell;
    zmq_recv(pullSocket, &receivedCell, sizeof(tCell), 0);

    // Print the values of the received struct
    printf("Received struct:\n");
    printf("Term: %s\n", receivedCell.term);
    printf("Term Length: %zu\n", receivedCell.term_length);

    // Cleanup
    zmq_close(pushSocket);
    zmq_close(pullSocket);
    zmq_ctx_destroy(context);

    return 0;
}
