#include "queue.h"
#include "randomService.h"
#include "stdio.h"
#include "string.h"
#include <pthread.h>
#include <time.h>

pthread_mutex_t mutex;

void *enqueueTerms(void *arg) {
    pthread_mutex_lock(&mutex);
    srand(time(NULL));
    wQueue *q = arg;
    char digest[HASH_LEN] = "";
    char *term;
    size_t len = 0;

    int i;
    for (i = 0; i < 1; i++) {
        term = createRandomString(term);
        len = strlen(term);
        hash(term, len, digest);
        enqueue(q, term, len, digest);
        writeToStorage(q, term, len, digest);
    }
    pthread_mutex_unlock(&mutex);
}

void *dequeueTerms(void *arg) {
    pthread_mutex_lock(&mutex);
    wQueue *q = arg;
    tCell *res = NULL;

    res = dequeue(q);
    while (res != NULL) {
        deleteFromStorage(res->digest);
        free(res->term);
        free(res);
        res = dequeue(q);
    }
    pthread_mutex_unlock(&mutex);
}

int main(void) {
    wQueue *q = init_queue();
    //readAllFromStorageToQueue(q);
    
    pthread_mutex_init(&mutex, NULL);

    pthread_t t1;
    pthread_create(&t1, NULL, &enqueueTerms, q);

    pthread_t t2;
    pthread_create(&t2, NULL, &dequeueTerms, q);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("\n\n----------------------------------------------------------------\n");
    printf("Length of Queue: %lu\n\n", q_size(q));
    printAllTermsOfCells(q);

    teardown_queue(q);
    return 0;
}
