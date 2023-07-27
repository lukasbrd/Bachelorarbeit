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

void test_sendAndPersist_persistOneTerm_called(void **state) {
    wQueue *q = init_queue();
    char *testterm = "testterm";
    int testcmd = ENQUEUE;
    char testtermdigest[HASH_LEN];
    size_t term_length = strlen(testterm);
    hash(testterm, (int)term_length, testtermdigest);
    zsock_t *commandSocket =  zsock_new_push("inproc://test");

    expect_string(__wrap_persistOneTerm, term, "testterm");
    expect_value(__wrap_persistOneTerm, term_length, 8);
    expect_memory(__wrap_persistOneTerm,digest,testtermdigest,HASH_LEN);

    sendAndPersist(commandSocket, testterm, testcmd, q);

    zsock_destroy(&commandSocket);
    free(q);
}

/*
void __wrap_zock_send(zsock_t *commandSocket,const char *picture,int cmd, tCell *cell ) {
    check_expected(commandSocket);
    check_expected(picture);
    check_expected(cmd);
    check_expected(cell);
}

static void test_sendAndPersist_zsock_send_called(void **state) {
    wQueue *q = init_queue();
    char *testterm = "testterm";
    int testcmd = ENQUEUE;
    zsock_t *commandSocket;

    expect_any(__wrap_zock_send, commandSocket);
    expect_value(__wrap_zock_send, picture, "ip");
    expect_value(__wrap_zock_send, testcmd, ENQUEUE);
    expect_any(__wrap_zock_send, cell);

    sendAndPersist(commandSocket, testterm, testcmd, q);
    free(q);
}*/