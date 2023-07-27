#ifndef TESTS_H
#define TESTS_H


void test_init_queue(void **state);
void test_sendAndPersist_persistOneTerm_called(void **state);
void __wrap_persistOneTerm(const char *term, int term_length, const char *digest);

#endif