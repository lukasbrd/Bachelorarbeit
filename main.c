#include "queue.h"
#include "randomService.h"
#include "stdio.h"
#include "string.h"
#include <pthread.h>
#include <time.h>


void* enqueueTerm(void* input) {
    struct Data* data = (struct Data*)input; 
    wQueue *q = data->q;
    char *term = data->term;
    /*
    size_t len = 0;
    char digest[HASH_LEN] = "";
    len = strlen(term);
    hash(term, len, digest);*/
    printf("Third:%s\n", term);
    free(term);

    //writeToStorage(term, len, digest);
    //enqueue(q, term, len, digest);
}




int main(void) {
    wQueue *q = init_queue();
    srand(time(NULL));
    char *term = NULL;
    term = createRandomString(term);
    printf("First:%s\n",term);
    struct Data data;

    tCell *res = NULL;
    data.q = q;
    data.term = term;
    printf("Second:%s\n",term);
    pthread_t enqueue;

    pthread_create(&enqueue, NULL, enqueueTerm, (void *) &data);
    pthread_join(enqueue, NULL);

    
    
/*
    while (q->c > 0) {
        if (q->c >= 3) {
            readOneTermFromStorageToQueue(q);
        }
        res = dequeue(q);
        free(res->term);
        free(res);
    }*/

    printf("\n\n----------------------------------------------------------------\n");
    printf("Length of Queue: %lu\n\n", q_size(q));
    //printAllTermsOfCells(q);
    printf("ENDE\n");

    teardown_queue(q);
    return 0;
}
