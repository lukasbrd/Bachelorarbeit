#include "../src/hash.h"
#include "../src/sha.h"
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "tests.h"
#include "../src/queue.h"

void test_init_queue(void **state) {
    wQueue *q = init_queue();
    assert_non_null(q);
    assert_null(q->first);
    assert_null(q->last);
    assert_int_equal(q->qlength, 0);
    free(q);
}

void test_init_cell(void **state) {
    char testterm[] = "Hello";
    char digest[HASH_LEN];
    size_t term_length = strlen(testterm);
    hash(testterm, (int) term_length, digest);
    tCell *cell1 = init_cell(testterm);

    assert_non_null(cell1);
    assert_string_equal(cell1->term, testterm);
    assert_int_equal(cell1->term_length, strlen(testterm));
    assert_memory_equal(cell1->digest,digest,term_length);
    assert_null(cell1->next);
    free(cell1);
}

void test_enqueueOneTerm(void **state) {
    wQueue *q = init_queue();
    char* testterm = "term";
    tCell *cell1 = init_cell(testterm);
    enqueue(q, cell1);

    assert_ptr_equal(q->first, cell1);
    assert_ptr_equal(q->last, cell1);

    free(cell1);
    free(q);
}

void test_enqueueTwoTerms(void **state) {
    wQueue *q = init_queue();
    char* testterm = "term";
    char* testterm2 = "term2";
    tCell *cell1 = init_cell(testterm);
    tCell *cell2 = init_cell(testterm2);


    enqueue(q, cell1);
    enqueue(q, cell2);

    assert_ptr_equal(q->first, cell1);
    assert_ptr_equal(q->last, cell2);
    assert_ptr_equal(q->first->next, cell2);
    assert_ptr_equal(q->last->next, NULL);

    free(cell1);
    free(cell2);
    free(q);
}
