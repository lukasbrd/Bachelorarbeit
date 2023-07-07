#include "queue.h"
#include "randomService.h"
#include "stdio.h"
#include "string.h"
#include <pthread.h>
#include <time.h>

int main(void) {
    wQueue *q = init_queue();
    // readAllFromStorageToQueue(q);
    srand(time(NULL));

    tCell *res = NULL;
    pthread_t threads[2];

    int i;
    for (i = 0; i < 2; i++) {
        char *term = createRandomString(term);
        size_t len = strlen(term);
        char digest[HASH_LEN] = "";
        hash(term, len, digest);

        StorageData *storageData = malloc(sizeof(StorageData));
        storageData->q = q;
        char *storageTerm = malloc(sizeof(char) * len + 1);
        strcpy(storageTerm, term);
        storageData->storageTerm = storageTerm;
        storageData->len = len;
        memcpy(storageData->digest, digest, 20);

        QueueData *queueData = malloc(sizeof(QueueData));
        queueData->q = q;
        char *queueTerm = malloc(sizeof(char) * len + 1);
        strcpy(queueTerm, term);
        queueData->queueTerm = queueTerm;
        queueData->len = len;
        memcpy(queueData->digest, digest, 20);

        pthread_create(&threads[0], NULL, writeToStorage, (void *)storageData);
        pthread_create(&threads[1], NULL, enqueue, (void *)queueData);
        free(term);
    }

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    while (q->c > 0) {
        if (q->c >= 2) {
            readOneTermFromStorageToQueue(q);
        }
        res = dequeue(q);
        free(res->term);
        free(res);
    }

    printf("\n\n----------------------------------------------------------------\n");
    printf("Length of Queue: %lu\n\n", q_size(q));
    printAllTermsOfCells(q);
    printf("ENDE\n");

    teardown_queue(q);
    return 0;
}
