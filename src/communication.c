#include "communication.h"
#include "persistenceInterface.h"
#include "settings.h"

void sendAndPersist(zsock_t *commandSocket, char *state, int cmd, Queue *q) {
    (q->qLength)++;
    Element *element = createElement(state);
    if (cmd != RESTORED) {
        persistOneState(element->state, element->stateLength, element->digest);
    }
    // printElement(element);
    if (q->qLength > INMEMORY) {
        element->state = NULL;
        free(state);
    }
    zsock_send(commandSocket, "ip", cmd, element);
}

Element *receiveAndRestore(zsock_t *commandSocket, zsock_t *packageSocket, Queue *q) {
    printf("DequeueLength: %d\n", q->qLength);
    Element *receivedElement = NULL;
    zsock_send(commandSocket, "ip", DEQUEUE, NULL);
    zsock_recv(packageSocket, "p", &receivedElement);
    if (receivedElement->state == NULL) {
        receivedElement->state = restoreOneState(receivedElement->digest);
    }
    (q->qLength)--;
    return receivedElement;
}