#include "queue.h"
#include "randomService.h"
#include "stdio.h"
#include "string.h"
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

bool exitThread1 = false;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *readOne(void *arg) {
    wQueue *q = (wQueue *)arg;

    while (!exitThread1) {
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond, &mutex);
        printf("Signal erhalten\n");
        readOneTermFromStorageToQueue(q);
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}

int main(void) {
    wQueue *q = init_queue();
    // readAllFromStorageToQueue(q);
    srand(time(NULL));
    char digest[HASH_LEN] = "";
    char *term;
    size_t len = 0;
    tCell *res = NULL;

    pthread_t thread1;
    int result = pthread_create(&thread1, NULL, readOne, (void *)q);

    int i;
    for (i = 0; i < 4; i++) {
        term = createRandomString(term);
        len = strlen(term);
        hash(term, len, digest);
        writeToStorage(term, len, digest);
        enqueue(q, term, len, digest);
    }

    while (q->c > 0) {
        if (q->c >= 3) {
            pthread_mutex_lock(&mutex);
            pthread_cond_signal(&cond);
            pthread_mutex_unlock(&mutex);
        }
        sleep(1);
        res = dequeue(q);
        if (res->term != NULL) {
            free(res->term);
        }
        free(res);
    }

    printf("EXITTHREAD\n");
    exitThread1 = true;

    printf("\n\n----------------------------------------------------------------\n");
    printf("Length of Queue: %lu\n\n", q_size(q));
    printAllTermsOfCells(q);
    printf("ENDE\n");

    teardown_queue(q);
    return 0;
}