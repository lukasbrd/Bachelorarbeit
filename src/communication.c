#include "communication.h"
#include "persistenceInterface.h"
#include "settings.h"
#include "queue.h"

void sendElement(zsock_t *commandSocket, char *state, int cmd, Queue *q) {
    q->qLength++;
    Element *element = createElement(state);
    zsock_send(commandSocket, "ip", cmd, element);
}

Element *receiveElement(zsock_t *commandSocket, zsock_t *packageSocket, Queue *q) {
    Element *receivedElement = NULL;
    zsock_send(commandSocket, "ip", DEQUEUE, NULL);
    zsock_recv(packageSocket, "p", &receivedElement);
    q->qLength--;
    return receivedElement;
}

