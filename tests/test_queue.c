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
    Queue *q = initQueue();
    assert_non_null(q);
    assert_null(q->first);
    assert_null(q->last);
    assert_int_equal(q->qLength, 0);
    free(q);
}

void test_create_cell(void **state) {
    char testTerm[] = "Hello";
    char digest[HASH_LEN];
    size_t term_length = strlen(testTerm);
    hash(testTerm, (int) term_length, digest);
    Element *element = createElement(testTerm);

    assert_non_null(element);
    assert_string_equal(element->state, testTerm);
    assert_int_equal(element->stateLength, strlen(testTerm));
    assert_memory_equal(element->digest, digest, term_length);
    assert_null(element->next);
    free(element);
}

void test_enqueueOneTerm(void **state) {
    Queue *q = initQueue();
    char* testTerm = "state";
    Element *element = createElement(testTerm);
    enqueue(q, element);

    assert_ptr_equal(q->first, element);
    assert_ptr_equal(q->last, element);

    free(element);
    free(q);
}

void test_enqueueTwoTerms(void **state) {
    Queue *q = initQueue();
    char* testTerm = "state";
    char* testTerm2 = "term2";
    Element *element1 = createElement(testTerm);
    Element *element2 = createElement(testTerm2);

    enqueue(q, element1);
    enqueue(q, element2);

    assert_ptr_equal(q->first, element1);
    assert_ptr_equal(q->last, element2);
    assert_ptr_equal(q->first->next, element2);
    assert_ptr_equal(q->last->next, NULL);

    free(element1);
    free(element2);
    free(q);
}

void test_dequeueOneTerm(void **state) {
    Queue *q = initQueue();
    char* testTerm = "state";
    Element *element = createElement(testTerm);
    char digest[HASH_LEN];
    size_t term_length = strlen(testTerm);
    hash(testTerm, (int)term_length, digest);

    enqueue(q, element);
    Element *res = dequeue(q);

    assert_ptr_equal(res->state, "state");
    assert_memory_equal(element->digest, digest, term_length);
    assert_int_equal(res->stateLength, strlen(testTerm));
    assert_null(res->next);

    free(element);
    free(q);
}