#ifndef AVRLAUNCH_BUFFER_TEST_H
#define AVRLAUNCH_BUFFER_TEST_H

void buffer_test_set_up(void);
void buffer_test_tear_down(void);

void test_push(void);
void test_push_overflow(void);
void test_push_no_overflow(void);
void test_no_overflow_shift(void);
void test_unused_elements_are_zero(void);

#endif
