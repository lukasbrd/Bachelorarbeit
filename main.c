#include "queue.h"
#include "randomService.h"
#include "stdio.h"
#include "string.h"
#include <time.h>

int main(void) {
    wQueue *q = init_queue();
    size_t len = 0;
    char digest[HASH_LEN] = "";
    char term[51];
    srand(time(NULL));
    int i;

    //Das Array hier erstellen und den Pointer dazu übergeben!!
    for (i = 0; i < 1; i++) {
        createRandomString(term);
        size_t len = strlen(term);
        hash(term, len, digest);
        //writeOnlyToBuffer(term,len,digest);
        enqueue(q, term, len, digest);
    }

    printf("\n\n----------------------------------------------------------------\n");
    printf("Length of Queue: %lu\n", q_size(q));

    // printf("Queue is empty: %d\n", is_empty(q));
    // printf("Size of q:  %ld\n", sizeof(wQueue));
    // printf("Size of size_t:  %ld\n", sizeof(size_t));

    teardown_queue(q);
    return 0;
}
