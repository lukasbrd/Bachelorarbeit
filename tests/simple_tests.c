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

static int setup(void **state) {
    return 0;
}

static int teardown(void **state) {
    return 0;
}


int main() {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test_setup_teardown(test_init_queue, setup, teardown),
            cmocka_unit_test_setup_teardown(test_sendAndPersist_persistOneTerm_called, setup, teardown),
            //cmocka_unit_test_setup_teardown(test_sendAndPersist_zsock_send_called, setup, teardown),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}

