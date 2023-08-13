#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "../src/queue.h"
#include "../src/hash.h"
#include "../src/sha.h"
#include <czmq.h>
#include "tests.h"

int main() {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_init_queue),
            cmocka_unit_test(test_init_cell),
            cmocka_unit_test(test_enqueueOneTerm),
            cmocka_unit_test(test_enqueueTwoTerms),
            cmocka_unit_test(test_sendAndPersist),
            cmocka_unit_test(test_dequeueOneTerm),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
