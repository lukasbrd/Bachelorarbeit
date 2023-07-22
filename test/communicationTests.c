#include "communicationTests.h"


wQueue *init_queueMock() {
    wQueue *q = (wQueue *)malloc(sizeof(wQueue));
    q->first = NULL;
    q->last = NULL;
    q->qlength = 0;
    return q;
}

void test_sendAndPersistQLength(void) {
    wQueue *qMock = init_queueMock();
    zsock_t *commandSocketMock = NULL;

    char *term = "TestTerm";
    int cmd = ENQUEUE;
    
    free(qMock);
}

/*
void test_sendAndPersistQLength2(void) {
    wQueue *qMock = init_queueMock();
    zsock_t *commandSocketMock = NULL;

    char *term = "TestTerm";
    char *term2 = "TestTerm2";
    int cmd = ENQUEUE;
    sendAndPersist(commandSocketMock, term, cmd, qMock);

    CU_ASSERT_EQUAL(qMock->qlength, 2);
}*/
