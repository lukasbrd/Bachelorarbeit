#include "communication.h"
#include "persistenceInterface.h"
#include "qthread.h"
#include "queue.h"
#include "state.h"
#include <czmq.h>
#include <stdio.h>
#include "settings.h"
#include <time.h>
#include "berkeleyDB.h"
#include "sqliteDB.h"

int main(void) {
    zsock_t *commandSocket = zsock_new_push("inproc://command");
    zsock_t *packageSocket = zsock_new_pull("inproc://package");
    Queue *q = initQueue();

    pthread_t thread;
    pthread_create(&thread, NULL, qthread, (void *)q);

    pthread_mutex_lock(&mutex);
    while (!threadRunning) {
        pthread_cond_wait(&condition, &mutex);
    }
    pthread_mutex_unlock(&mutex);

#ifdef BERKELEYDB
    initBerkeleyDB();
#endif

#ifdef SQLITE
    initSqliteDB();
#endif

#ifdef RESTOREALLSTATESTOQUEUE
    restoreAllStatesToQueue(commandSocket, q);
#endif

    srand(0);
    for (int i = 1; i <= NUMBEROFSTATES; i++) {
        char *state = createState(i);
        //printf("Start:%s\n", state);
        enqueueElementWithState(commandSocket, state, NEWSTATE, q);
     }

    while (q->qLength > 0) {
        Element *dequeuedElement;
        dequeuedElement = dequeueElementWithState(commandSocket, packageSocket, q);
#ifdef DELETESTATESAFTEREXECUTION
        deleteOneStateFromPersistentStorage(dequeuedElement);
#endif
        //printf("Ende: %s\n", dequeuedElement->state);
        free(dequeuedElement->state);
        free(dequeuedElement);
    }

    zsock_send(commandSocket, "ip", TERMINATE, NULL);
    pthread_join(thread, NULL);
    zsock_destroy(&commandSocket);
    zsock_destroy(&packageSocket);
    free(q);

#ifdef BERKELEYDB
    closeBerkeleyDB();
#endif

#ifdef SQLITE
    closeSQLiteDB();
#endif
    return 0;
}
