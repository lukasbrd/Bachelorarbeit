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