#include "fileInputOutput.h"
#include "queue.h"
#include "stdio.h"

int main(void) {
    wQueue *q = init_queue();
    int read = 0;
    int write = 0;

    char *term = "a";
    char *term2 = "bc";
    char *term3 = "def";

    char *term4 = "ghij";

    enqueue(q, term, 1);
    write = writeToFile(term, read, write);

    enqueue(q, term2, 2);
    write = writeToFile(term2,read,write);


    enqueue(q, term3, 3);
    write = writeToFile(term3,read,write);


    enqueue(q, term4, 4);
    write = writeToFile(term4,read,write);


    printf("Length of Queue: %lu\n", q_size(q));
    printf("Queue is empty: %d\n", is_empty(q));
    printf("Size of q:  %ld\n", sizeof(wQueue));
    printf("Size of size_t:  %ld\n", sizeof(size_t));

    printAllTermsOfCells(q);
    teardown_queue(q);

    return 0;
}
