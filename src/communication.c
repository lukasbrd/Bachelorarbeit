#include "communication.h"
#include "persistenceInterface.h"
#include "settings.h"
#include "queue.h"

void enqueueElementWithState(zsock_t *commandSocket, char *state, int command, Queue *q) {
    q->qLength++;
    Element *element = createElement(state);
    zsock_send(commandSocket, "ip", command, element);
}

Element *dequeueElementWithState(zsock_t *commandSocket, zsock_t *packageSocket, Queue *q) {
    Element *dequeuedElement = NULL;
    zsock_send(commandSocket, "ip", DEQUEUE, NULL);
    zsock_recv(packageSocket, "p", &dequeuedElement);
    q->qLength--;
    return dequeuedElement;
}

