#include "queue.h"
#include "randomService.h"
#include "stdio.h"
#include "string.h"
#include <time.h>

int main(void) {
    wQueue *q = init_queue();

    srand(time(NULL));
    int i;
    for (i = 0; i < 2; i++) {
        char *const randomString = createRandomString();
        size_t len = strlen(randomString);
        char digest[HASH_LEN];
        sha1(randomString, len, digest);
        enqueue(q, randomString, len, digest);
    }


    printAllTermsOfCells(q);
    printf("Length of Queue: %lu\n", q_size(q));

    // printf("Queue is empty: %d\n", is_empty(q));
    // printf("Size of q:  %ld\n", sizeof(wQueue));
    // printf("Size of size_t:  %ld\n", sizeof(size_t));

    teardown_queue(q);
    return 0;
}
