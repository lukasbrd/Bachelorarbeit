#include "communication.h"
#include "persistenceInterface.h"
#include "settings.h"

void sendAndPersist(zsock_t *commandSocket, char *state, int cmd, Queue *q) {
    q->qLength++;
    printf("qLengthPlus %d\n",q->qLength);
    Element *element = createElement(state);
    zsock_send(commandSocket, "ip", cmd, element);
}

Element *receiveAndRestore(zsock_t *commandSocket, zsock_t *packageSocket, Queue *q) {
    Element *receivedElement = NULL;
    zsock_send(commandSocket, "ip", DEQUEUE, NULL);
    zsock_recv(packageSocket, "p", &receivedElement);
    q->qLength--;
    printf("qLengthMinus %d\n",q->qLength);
    return receivedElement;
}