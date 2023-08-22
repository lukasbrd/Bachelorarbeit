#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "../src/communication.h"
#include "tests.h"


void __wrap_persistOneTerm(const char *term, int term_length, const char *digest) {
    check_expected(term);
    check_expected(term_length);
    check_expected(digest);
}


void __wrap_zsock_send(zsock_t *commandSocket, const char *picture, int cmd, Element *cell ) {
    check_expected(commandSocket);
    check_expected(picture);
    check_expected(cmd);
    check_expected(cell);
}

void test_sendAndPersist(void **state) {
    Queue *q = initQueue();
    char *testterm = "testterm";
    int testcmd = ENQUEUE;
    char testtermdigest[HASH_LEN];
    size_t term_length = strlen(testterm);
    hash(testterm, (int)term_length, testtermdigest);
    zsock_t *commandSocket =  zsock_new_push("inproc://test1");
    Element *cell = createElement(testterm);

    expect_string(__wrap_persistOneTerm, term, testterm);
    expect_value(__wrap_persistOneTerm, term_length, 8);
    expect_memory(__wrap_persistOneTerm,digest,testtermdigest,HASH_LEN);

    expect_any(__wrap_zsock_send, commandSocket);
    expect_string(__wrap_zsock_send, picture, "ip");
    expect_value(__wrap_zsock_send, cmd, testcmd);
    expect_any(__wrap_zsock_send, cell);

    sendElement(commandSocket, testterm, testcmd, q);

    zsock_destroy(&commandSocket);
    free(q);
}







