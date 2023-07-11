#include "queue.h"
#include "randomService.h"
#include "stdio.h"
#include "string.h"
#include <pthread.h>
#include <time.h>

void enqueueTerm(wQueue *const q, char *const term, const size_t len, char digest[HASH_LEN]) {
    q->c++;
    writeToStorage(term, len, digest);
    if (q->c < 3) {
        enqueue(q, term, len, digest);
    } else {
        enqueueWithoutTerm(q, term, len, digest);
    }
}

tCell *dequeueTerm(wQueue *const q) {
    if (q->c == 0) {
        return NULL;
    }
    if (q->c >= 3) {
        readOneTermFromStorageToQueue(q);
        if (q->first_not_in_mem->next != NULL) {
            q->first_not_in_mem = q->first_not_in_mem->next;
        }
    }
    q->c--;
    return dequeue(q);
}

int main(void) {
    wQueue *q = init_queue();
    // readAllFromStorageToQueue(q);
    srand(time(NULL));
    char digest[HASH_LEN] = "";
    char *term;
    size_t len = 0;
    tCell *res = NULL;

    int i;
    for (i = 0; i < 4; i++) {
        term = createRandomString(term);
        len = strlen(term);
        hash(term, len, digest);
        enqueueTerm(q, term, len, digest);
    }

    while (q->c > 0) {
        res = dequeueTerm(q);
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
