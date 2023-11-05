#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "../src/qthread.h"
#include "../src/persistenceInterface.h"
#include "tests.h"

void __wrap_restoreOneState(Element *dequeuedElement) {
    check_expected(dequeuedElement);
}

void test_deleteStateIfMemFull1(void **state) {
    Queue *q = initQueue();
    q->in_mem = 2;
    char testTerm[] = "testTerm";
    char *dynamicallyAllocatedTestTerm = strdup(testTerm);

    Element *element = createElement(dynamicallyAllocatedTestTerm);
    deleteStateIfMemFull(q,element);

    assert_null(element->state);
    assert_int_equal(q->not_in_mem,1);
    assert_int_equal(q->in_mem,2);

    free(element);
    free(q);
}

void test_deleteStateIfMemFull2(void **state) {
    Queue *q = initQueue();
    char testTerm[] = "testTerm";
    char *dynamicallyAllocatedTestTerm = strdup(testTerm);

    Element *element = createElement(dynamicallyAllocatedTestTerm);
    deleteStateIfMemFull(q,element);

    assert_non_null(element->state);
    assert_int_equal(q->not_in_mem,0);
    assert_int_equal(q->in_mem,1);

    free(element);
    free(q);
}

void test_restoreStateIfItWasDeleted1(void **state) {
    Queue *q = initQueue();
    q->not_in_mem = 1;
    Element *dequeuedElement = createElement("");
    dequeuedElement->state = NULL;

    expect_any(__wrap_restoreOneState, dequeuedElement);
    restoreStateIfItWasDeleted(q, dequeuedElement);
    assert_int_equal(q->not_in_mem,0);
    free(q);
}

void test_restoreStateIfItWasDeleted2(void **state) {
    Queue *q = initQueue();
    q->in_mem = 1;
    Element *dequeuedElement = createElement("testTerm");

    restoreStateIfItWasDeleted(q, dequeuedElement);
    assert_int_equal(q->not_in_mem,0);

    free(q);
}