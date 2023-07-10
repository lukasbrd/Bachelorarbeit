#include "queue.h"
#include "randomService.h"
#include "stdio.h"
#include "string.h"
#include <pthread.h>
#include <stdatomic.h>
#include <time.h>

void *enqueueTerm(void *input) {
    struct Data *data = (struct Data *)input;
    wQueue *q = data->q;
    char *term = data->term;

    size_t len = 0;
    char digest[HASH_LEN] = "";
    len = strlen(term);
    hash(term, len, digest);
    printf("Third:%s\n", term);

    writeToStorage(term, len, digest);
    enqueue(q, term, len, digest);
}

void *dequeueTerm(void *input) {
    struct Data *data = (struct Data *)input;
    tCell *res = NULL;
    wQueue *q = data->q;
    if (atomic_load(&q->not_in_mem) > 0) {
        readOneTermFromStorageToQueue(q);
        atomic_fetch_sub(&q->not_in_mem, 1);
        atomic_fetch_add(&q->in_mem, 1);
    }
    res = dequeue(q);
    free(res->term);
    free(res);
    return res;
}

int main(void) {
    wQueue *q = init_queue();
    srand(time(NULL));

    pthread_t enqueue;
    pthread_t dequeue;

    for (int i = 0; i < 4; i++) {
        char *term = createRandomString(term);
        struct Data data;
        printf("First:%s\n", term);
        data.q = q;
        data.term = term;
        printf("Second:%s\n", term);

        pthread_create(&enqueue, NULL, enqueueTerm, (void *)&data);
        pthread_join(enqueue, NULL);
        pthread_create(&dequeue, NULL, dequeueTerm, (void *)&data);
        pthread_join(dequeue, NULL);
    }

    printf("\n\n----------------------------------------------------------------\n");
    printf("Length of Queue: %lu\n\n", q_size(q));
    printAllTermsOfCells(q);
    printf("ENDE\n");

    teardown_queue(q); // freed den Rest der Queue und die Q selbst!!!
    return 0;
}
