#ifndef AVRLAUNCH_BUFFER_TEST_H
#define AVRLAUNCH_BUFFER_TEST_H

void buffer_test_set_up(void);
void buffer_test_tear_down(void);

void test_should_add_to_fixed_buffer(void);
void test_should_add_to_circular_buffer(void);
void test_fixed_buffer_exceeds_size(void);
void test_circular_buffer_exceeds_size(void);

#endif
