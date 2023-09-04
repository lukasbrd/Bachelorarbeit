#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "../src/communication.h"
#include "tests.h"

void __wrap_zsock_send(zsock_t *commandSocket, char *picture, int cmd, Element *element) {
    check_expected(commandSocket);
    check_expected(picture);
    check_expected(cmd);
    check_expected(element);
}

void __wrap_zsock_recv(zsock_t *packageSocket, char *picture, Element *receivedElement) {
    check_expected(packageSocket);
    check_expected(picture);
    check_expected_ptr(receivedElement);
}

void test_sendElement(void **state) {
    Queue *q = initQueue();
    char *term = "term";
    zsock_t *commandSocket = zsock_new_push("inproc://pushtest");
    Element *element = createElement(term);

    expect_any(__wrap_zsock_send, commandSocket);
    expect_string(__wrap_zsock_send, picture, "ip");
    expect_value(__wrap_zsock_send, cmd, ENQUEUE);
    expect_any(__wrap_zsock_send, element);

    sendElement(commandSocket, term, ENQUEUE, q);

    assert_int_equal(q->qLength,1);
    zsock_destroy(&commandSocket);
    free(q);
}

void test_receiveElement(void **state) {
    Queue *q = initQueue();
    q->qLength = 1;
    void *element = NULL;
    Element *receivedElement = NULL;
    zsock_t *commandSocket = zsock_new_push("inproc://pushtest");
    zsock_t *packageSocket = zsock_new_pull("inproc://pulltest");

    expect_any(__wrap_zsock_send, commandSocket);
    expect_string(__wrap_zsock_send, picture, "ip");
    expect_value(__wrap_zsock_send, cmd, DEQUEUE);
    expect_any(__wrap_zsock_send, element);


    expect_any(__wrap_zsock_recv, packageSocket);
    expect_string(__wrap_zsock_recv, picture, "p");
    expect_any(__wrap_zsock_recv, receivedElement);

    receiveElement(commandSocket,packageSocket,q);

    assert_int_equal(q->qLength,0);

    zsock_destroy(&commandSocket);
    zsock_destroy(&packageSocket);
    free(q);
}







