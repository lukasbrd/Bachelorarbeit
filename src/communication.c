#include "communication.h"
#include "persistenceInterface.h"
#include "settings.h"



void sendAndPersist(zsock_t *commandSocket, char *term, int cmd, wQueue *q) {
    (q->qlength)++;
    tCell *cell = init_cell(term);
    if (cmd == ENQUEUE) {
        persistOneTerm(cell->term, cell->term_length, cell->digest);
    }
    // printCell(cell);
    if (q->qlength > INMEMORY) {
        cell->term = NULL;
        free(term);
    }
    zsock_send(commandSocket, "ip", cmd, cell);
}

tCell *receiveAndRestore(zsock_t *command, zsock_t *packageSocket, wQueue *q) {
    printf("DequeueLength: %d\n", q->qlength);
    if (q->qlength == 0) {
        return NULL;
    }
    tCell *receivedCell = NULL;
    zsock_send(command, "ip", DEQUEUE, NULL);
    int rc = zsock_recv(packageSocket, "p", &receivedCell);
    assert(rc == 0);
    if (receivedCell->term == NULL) {
        receivedCell->term = loadOneTerm(receivedCell->digest);
    }
    (q->qlength)--;
    return receivedCell;
}