#define UNITY_FIXTURE_MALLOC_OVERRIDES_H_

#include <stdlib.h>
#include <string.h>

#include "unity.h"

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/buffer.h"

#include "avrlaunch/test/buffer_test.h"

#define TEST_BUFFER_SIZE 10

static struct buffer test_buffer;
static uint8_t test_values[TEST_BUFFER_SIZE];

void buffer_test_set_up() {
	memset(test_values, 0, TEST_BUFFER_SIZE);
}

void buffer_test_tear_down() {}

void test_should_add_to_fixed_buffer(void) {
	buffer_init(&test_buffer, test_values, TEST_BUFFER_SIZE, BUFFER_FIXED);
	buffer_push(&test_buffer, 34);
	buffer_push(&test_buffer, 100);
	buffer_push(&test_buffer, 211);
	TEST_ASSERT_EQUAL_UINT(3, test_buffer.count);	
	TEST_ASSERT_EQUAL_UINT(34, buffer_at(&test_buffer, 0));	
	TEST_ASSERT_EQUAL_UINT(100, buffer_at(&test_buffer, 1));	
	TEST_ASSERT_EQUAL_UINT(211, buffer_at(&test_buffer, 2));	
}

void test_should_add_to_circular_buffer(void) {
	buffer_init(&test_buffer, test_values, TEST_BUFFER_SIZE, BUFFER_CIRCULAR);
	buffer_push(&test_buffer, 83);
	buffer_push(&test_buffer, 255);
	buffer_push(&test_buffer, 2);
	TEST_ASSERT_EQUAL_UINT(3, test_buffer.count);	
	TEST_ASSERT_EQUAL_UINT(83, buffer_at(&test_buffer, 0));	
	TEST_ASSERT_EQUAL_UINT(255, buffer_at(&test_buffer, 1));	
	TEST_ASSERT_EQUAL_UINT(2, buffer_at(&test_buffer, 2));	
}

void test_fixed_buffer_exceeds_size(void) {
	buffer_init(&test_buffer, test_values, TEST_BUFFER_SIZE, BUFFER_FIXED);
	for(int i = 0; i < 20; i++) {
		buffer_push(&test_buffer, i * 2);	
	}
	TEST_ASSERT_EQUAL_UINT(10, test_buffer.count);
	TEST_ASSERT_EQUAL_UINT(0, buffer_at(&test_buffer, 0));	
	TEST_ASSERT_EQUAL_UINT(18, buffer_at(&test_buffer, 9));	
}

void test_circular_buffer_exceeds_size(void) {
	buffer_init(&test_buffer, test_values, TEST_BUFFER_SIZE, BUFFER_CIRCULAR);
	for(int i = 0; i < 20; i++) {
		buffer_push(&test_buffer, i * 2);	
	}
	TEST_ASSERT_EQUAL_UINT(10, test_buffer.count);
	TEST_ASSERT_EQUAL_UINT(20, buffer_at(&test_buffer, 0));	
	TEST_ASSERT_EQUAL_UINT(38, buffer_at(&test_buffer, 9));	
}
