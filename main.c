#include "queue.h"
#include "randomService.h"
#include "stdio.h"
#include "string.h"
#include <pthread.h>
#include <time.h>

pthread_mutex_t mutex;

void *enqueueTerm(wQueue *const q, char *const term, const size_t len, const char digest[HASH_LEN]) {
    if (q->c < 2) {
        enqueue(q, term, len, digest);
        writeToStorage(q, term, len, digest);
    } else {
        enqueueWithoutTerm(q, term, len, digest);
        writeToStorage(q, term, len, digest);
    }
}

tCell *dequeueTerm(void *arg) {
    wQueue *q = arg;
    tCell *res = NULL;

    if (q->c > 0 && q->c < 2) {
        res = dequeue(q);
    } else if(q->c >= 2) {
        readOneTermFromStorageToQueue(q);
        res = dequeue(q);
    }
    return res;
}

int main(void) {
    wQueue *q = init_queue();
    //readAllFromStorageToQueue(q);
    srand(time(NULL));
    char digest[HASH_LEN] = "";
    char *term;
    size_t len = 0;
    tCell *res = NULL;

    int i;
    for (i = 0; i < 3; i++) {
        term = createRandomString(term);
        len = strlen(term);
        hash(term, len, digest);
        enqueueTerm(q, term, len, digest);
    }

    while (q->c != 0) {
        res = dequeueTerm(q);
        free(res->term);
        free(res);
    }

    printf("\n\n----------------------------------------------------------------\n");
    printf("Length of Queue: %lu\n\n", q_size(q));
    printAllTermsOfCells(q);

    teardown_queue(q);
    return 0;
}
