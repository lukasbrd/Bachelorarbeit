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
    
    // Thread1:
    int i;
    for (i = 0; i < 10; i++) {
        term = createRandomString(term);
        len = strlen(term);
        hash(term, len, digest);
        if (q->c < 5) {
            enqueue(q, term, len, digest);
            writeToBuffer(q, term, len, digest);
        } else {
            writeToBuffer(q,term,len,digest);
            free(term);
        }
    }
    
    // Thread2:
    readFromBufferToQueue(q);
    deleteFromBuffer(q,q->first->digest);
    res = dequeue(q);
    free(res);

    readFromBufferToQueue(q);
    deleteFromBuffer(q, q->first->digest);
    res = dequeue(q);
    free(res);

    printf("\n\n----------------------------------------------------------------\n");
    printf("Length of Queue: %lu\n", q_size(q));
    printAllTermsOfCells(q);

    teardown_queue(q);
    return 0;
}
