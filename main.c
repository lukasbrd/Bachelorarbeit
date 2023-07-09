#include "queue.h"
#include "randomService.h"
#include "stdio.h"
#include "string.h"
#include <pthread.h>
#include <time.h>

wQueue *q;

void *enqueueThread(void *arg) {
    char **terms = (char **)arg;
    for (int i = 0; i < 4; i++) {
        printf("TERM: %s\n", terms[i]);
        free(terms[i]);
    }
    pthread_exit(NULL);
}

int main(void) {
    wQueue *q = init_queue();
    //readAllFromStorageToQueue(q);
    srand(time(NULL));
    
    tCell *res = NULL;
    char* terms[4];
    
    pthread_t thread;

    int i;
    for (i = 0; i < 4; i++) {
        terms[i] = createRandomString();
        printf("TERM: %s\n",terms[i]);        //print funktioniert!!
    }
    
    if (pthread_create(&thread, NULL, enqueueThread, (void *)terms) != 0) {
        printf("Failed to create thread.\n");
        return 1;
    }

    if (pthread_join(thread, NULL) != 0) {
        printf("Failed to join thread.\n");
        return 1;
    }

    /*
    for (i = 0; i < 4; i++) {   
        len[i] = strlen(terms[i]);
        hash(term, len, digest);

        enqueue(q, term, len, digest);
    }*/

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
    //printf("Length of Queue: %lu\n\n", q_size(q));
    //printAllTermsOfCells(q);
    printf("ENDE\n");

    //teardown_queue(q);
    return 0;
}
