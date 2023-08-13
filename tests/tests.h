#ifndef TESTS_H
#define TESTS_H

#include "../src/communication.h"

void test_init_queue(void **state);
void test_init_cell(void **state);
void test_enqueueOneTerm(void **state);
void test_enqueueTwoTerms(void **state);
void test_dequeueOneTerm(void **state);


void test_sendAndPersist(void **state);



void __wrap_persistOneTerm(const char *term, int term_length, const char *digest);
void __wrap_zsock_send(zsock_t *commandSocket, const char *picture, int cmd, Element *cell );




#endif