#include "queueTests.h"

void test_init_q() {
    wQueue *queue = init_queue();
    CU_ASSERT_PTR_NOT_NULL(queue);
    CU_ASSERT_PTR_NULL(queue->first);
    CU_ASSERT_PTR_NULL(queue->last);
    CU_ASSERT_EQUAL(queue->qlength, 0);
    free(queue);
}

void test_init_cell() {
    char *term = "Term1";
    tCell *cell = init_cell(term);
    CU_ASSERT_PTR_NOT_NULL(cell);
    CU_ASSERT_STRING_EQUAL(cell->term, term);
    CU_ASSERT_EQUAL(cell->term_length, strlen(term));
}

void test_enqueueOneTerm() {
    wQueue *queue = init_queue();
    char* term = "testterm";
    tCell *cell1 = init_cell(term);

    enqueue(queue, cell1);
    
    CU_ASSERT(queue->first == cell1);
    CU_ASSERT(queue->last == cell1);
    CU_ASSERT(queue->first->next == NULL);
}

void test_enqueueTwoTerms() {
    wQueue *queue = init_queue();
    char *term = "testterm";
    char *term2 = "testterm2";

    tCell *cell1 = init_cell(term);
    tCell *cell2 = init_cell(term2);

    enqueue(queue, cell1);
    enqueue(queue, cell2);

    CU_ASSERT(queue->first == cell1);
    CU_ASSERT(queue->last == cell2);
    CU_ASSERT(queue->first->next == cell2);
    CU_ASSERT(queue->last->next == NULL);
}