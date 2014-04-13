#ifndef AVRLAUNCH_BUFFER_INT_TEST_H
#define AVRLAUNCH_BUFFER_INT_TEST_H

void buffer_int_test_set_up(void);
void buffer_int_test_tear_down(void);

void test_push_uint8(void);
void test_push_overflow_uint8(void);
void test_push_no_overflow_uint8(void);
void test_no_overflow_shift_uint8(void);
void test_push_uint16(void);
void test_push_overflow_uint16(void);
void test_push_no_overflow_uint16(void);
void test_no_overflow_shift_uint16(void);

#endif
