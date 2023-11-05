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
            cmocka_unit_test(test_create_cell),
            cmocka_unit_test(test_enqueueOneTerm),
            cmocka_unit_test(test_enqueueTwoTerms),
            cmocka_unit_test(test_dequeueOneTerm),
            cmocka_unit_test(test_sendElement),
            cmocka_unit_test(test_receiveElement),
            cmocka_unit_test(test_deleteStateIfMemFull1),
            cmocka_unit_test(test_deleteStateIfMemFull2),
            cmocka_unit_test(test_restoreStateIfItWasDeleted1),
            cmocka_unit_test(test_restoreStateIfItWasDeleted2),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
