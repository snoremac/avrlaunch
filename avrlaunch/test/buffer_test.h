#ifndef AVRLAUNCH_BUFFER_TEST_H
#define AVRLAUNCH_BUFFER_TEST_H

void buffer_test_set_up(void);
void buffer_test_tear_down(void);

void test_add_fixed_uint8(void);
void test_add_circular_uint8(void);
void test_overflow_fixed_uint8(void);
void test_overflow_circular_uint8(void);

void test_add_fixed_uint16(void);
void test_add_circular_uint16(void);
void test_overflow_fixed_uint16(void);
void test_overflow_circular_uint16(void);

void test_unused_elements_are_zero(void);

#endif
