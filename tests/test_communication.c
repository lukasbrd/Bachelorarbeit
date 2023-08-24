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

void test_sendElement(void **state) {
    Queue *q = initQueue();
    char *term = "term";
    int cmd = ENQUEUE;
    char digest[HASH_LEN];
    size_t stateLength = strlen(term);
    hash(term, (int)stateLength, digest);
    zsock_t *commandSocket = zsock_new_push("inproc://test");
    Element *cell = createElement(term);

    expect_any(__wrap_zsock_send, commandSocket);
    expect_string(__wrap_zsock_send, picture, "ip");
    expect_value(__wrap_zsock_send, cmd, cmd);
    expect_any(__wrap_zsock_send, cell);

    sendElement(commandSocket, term, cmd, q);

    assert_int_equal(q->qLength,1);
    zsock_destroy(&commandSocket);
    free(q);
}







