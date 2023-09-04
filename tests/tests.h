#ifndef TESTS_H
#define TESTS_H

#include "../src/communication.h"

void test_init_queue(void **state);
void test_create_cell(void **state);
void test_enqueueOneTerm(void **state);
void test_enqueueTwoTerms(void **state);
void test_dequeueOneTerm(void **state);
void test_sendElement(void **state);
void test_writeOneStateToSQLiteDatabase(void **state);
void test_receiveElement(void **state);

int test_setup(void **state);
int test_teardown(void **state);

void wrap_persistOneTerm(const char *term, int term_length, const char *digest);
void wrap_zsock_send(zsock_t *commandSocket, const char *picture, int cmd, Element *cell);
char *wrap_get_database_name();

#endif