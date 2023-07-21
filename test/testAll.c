#include "CUnit/Basic.h"
#include "initTests.h"

int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

int main(void) {
    //create a test suite
    CU_pSuite suite1 = NULL;

    //initialize the cunit registry
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    suite1 = CU_add_suite("suite1", init_suite, clean_suite);
    if (NULL == suite1) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ((NULL == CU_add_test(suite1, "\n\ninit_Cell\n\n", test_init_cell))) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ((NULL == CU_add_test(suite1, "\n\ninit_q\n\n", test_init_q))) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}