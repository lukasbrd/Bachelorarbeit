#include "fileInputOutput.h"
#include "queue.h"
#include "stdio.h"
#include "string.h"

int main(void) {
    wQueue *q = init_queue();


    char *term = "abcd";
    size_t len = strlen(term);
    char digest[HASH_LEN];
    sha1(term,len,digest);


    enqueue(q, term, len, digest);


    printf("Length of Queue: %lu\n", q_size(q));
    printf("Queue is empty: %d\n", is_empty(q));
    printf("Size of q:  %ld\n", sizeof(wQueue));
    printf("Size of size_t:  %ld\n", sizeof(size_t));

    printAllTermsOfCells(q);
    teardown_queue(q);

    return 0;
}
