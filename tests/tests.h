#ifndef TESTS_H
#define TESTS_H

#include "../src/communication.h"

void test_init_queue(void **state);
void test_create_cell(void **state);
void test_enqueueOneTerm(void **state);
void test_enqueueTwoTerms(void **state);
void test_dequeueOneTerm(void **state);
void test_sendElement(void **state);
void test_receiveElement(void **state);
void test_deleteStateIfMemFull1(void **state);
void test_deleteStateIfMemFull2(void **state);
void test_restoreStateIfItWasDeleted1(void **state);
void test_restoreStateIfItWasDeleted2(void **state);

#endif