#include "initTests.h"

void test_init_q() {
    wQueue *q = init_queue();
    CU_ASSERT_PTR_NOT_NULL(q);
    CU_ASSERT_PTR_NULL(q->first);
    CU_ASSERT_PTR_NULL(q->last);
    CU_ASSERT_EQUAL(q->qlength, 0);
    free(q);
}

void test_init_cell() {
    char *term = "Term1";
    tCell *cell = init_cell(term);
    CU_ASSERT_PTR_NOT_NULL(cell);
    CU_ASSERT_STRING_EQUAL(cell->term, term);
    CU_ASSERT_EQUAL(cell->term_length, strlen(term));
}

