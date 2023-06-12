#include "queue.h"
#include "randomService.h"
#include "stdio.h"
#include "string.h"
#include <time.h>

int main(void) {
    wQueue *q = init_queue();
    size_t len = 0;
    tCell *res = NULL;
    char digest[HASH_LEN] = "";
    char *term;
    srand(time(NULL));

    readAllFromStorageToQueue(q); 

    /*
    // Thread1:
    int i;
    for (i = 0; i < 3; i++) {
        term = createRandomString(term);
        len = strlen(term);
        hash(term, len, digest);
        enqueue(q, term, len, digest);
        writeToStorage(q, term, len, digest);
    }

    // Thread2:
    res = dequeue(q);
    deleteFromStorage(res->digest);
    free(res->term);
    free(res);
    */

    printf("\n\n----------------------------------------------------------------\n");
    printf("Length of Queue: %lu\n\n", q_size(q));
    printAllTermsOfCells(q);

    teardown_queue(q);
    return 0;
}
