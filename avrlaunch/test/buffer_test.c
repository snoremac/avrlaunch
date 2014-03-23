#include <stdlib.h>
#include <string.h>

#include "unity.h"

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/buffer.h"

#include "avrlaunch/test/buffer_test.h"

#define BUFFER_MAX_ELEMENTS 10

static const uint8_t test_values_uint8[15] = {
	43, 213, 119, 80, 2, 78, 200, 189, 14, 167, 101, 32, 9, 229, 67
};

static const uint16_t test_values_uint16[15] = {
	6237, 23890, 23, 931, 61001, 278, 448, 11987, 1089, 2299, 467, 1730, 48500, 39899, 7
};

static void test_elements_uint8(buffer_mode mode);
static void test_overflow_uint8(buffer_mode mode);
static void test_elements_uint16(buffer_mode mode);
static void test_overflow_uint16(buffer_mode mode);

void buffer_test_set_up() {}

void buffer_test_tear_down() {}

void test_add_fixed_uint8() {
	test_elements_uint8(BUFFER_FIXED);
}

void test_add_circular_uint8() {
	test_elements_uint8(BUFFER_CIRCULAR);
}

static void test_elements_uint8(buffer_mode mode) {
	struct buffer buffer;
	uint8_t buffer_data[BUFFER_MAX_ELEMENTS];
	buffer_init(&buffer, buffer_data, BUFFER_MAX_ELEMENTS, sizeof(uint8_t), mode);
	uint8_t elements_added = 5;

	for (uint8_t i = 0; i < elements_added; i++) {
		buffer_push(&buffer, &test_values_uint8[i]);
	}

	TEST_ASSERT_EQUAL_UINT(elements_added, buffer.element_count);	
	for (uint8_t i = 0; i < elements_added; i++) {
		TEST_ASSERT_EQUAL_UINT(test_values_uint8[i], buffer_uint8_at(&buffer, i));
	}	
}

void test_add_fixed_uint16(void) {
	test_elements_uint16(BUFFER_FIXED);
}

void test_add_circular_uint16(void) {
	test_elements_uint16(BUFFER_CIRCULAR);
}

static void test_elements_uint16(buffer_mode mode) {
	struct buffer buffer;
	uint16_t buffer_data[BUFFER_MAX_ELEMENTS];
	buffer_init(&buffer, buffer_data, BUFFER_MAX_ELEMENTS, sizeof(uint16_t), mode);
	uint16_t elements_added = 5;

	for (uint16_t i = 0; i < elements_added; i++) {
		buffer_push(&buffer, &test_values_uint16[i]);
	}

	TEST_ASSERT_EQUAL_UINT(elements_added, buffer.element_count);	
	for (uint16_t i = 0; i < elements_added; i++) {
		TEST_ASSERT_EQUAL_UINT(test_values_uint16[i], buffer_uint16_at(&buffer, i));
	}	
}

void test_overflow_fixed_uint8() {
	test_overflow_uint8(BUFFER_FIXED);
}

void test_overflow_circular_uint8() {
	test_overflow_uint8(BUFFER_CIRCULAR);
}

static void test_overflow_uint8(buffer_mode mode) {
	struct buffer buffer;
	uint8_t buffer_data[BUFFER_MAX_ELEMENTS];
	buffer_init(&buffer, buffer_data, BUFFER_MAX_ELEMENTS, sizeof(uint8_t), mode);
	uint8_t elements_added = 15;

	for(int i = 0; i < elements_added; i++) {
		buffer_push(&buffer, &test_values_uint8[i]);
	}

	TEST_ASSERT_EQUAL_UINT(BUFFER_MAX_ELEMENTS, buffer.element_count);
	if (mode == BUFFER_FIXED) {
		TEST_ASSERT_EQUAL_UINT(test_values_uint8[0], buffer_uint8_at(&buffer, 0));
		TEST_ASSERT_EQUAL_UINT(test_values_uint8[9], buffer_uint8_at(&buffer, 9));
	} else {
		TEST_ASSERT_EQUAL_UINT(test_values_uint8[5], buffer_uint8_at(&buffer, 0));
		TEST_ASSERT_EQUAL_UINT(test_values_uint8[14], buffer_uint8_at(&buffer, 9));
	}
}

void test_overflow_fixed_uint16() {
	test_overflow_uint16(BUFFER_FIXED);
}

void test_overflow_circular_uint16() {
	test_overflow_uint16(BUFFER_CIRCULAR);
}

static void test_overflow_uint16(buffer_mode mode) {
	struct buffer buffer;
	uint16_t buffer_data[BUFFER_MAX_ELEMENTS];
	buffer_init(&buffer, buffer_data, BUFFER_MAX_ELEMENTS, sizeof(uint16_t), mode);
	uint16_t elements_added = 15;

	for(int i = 0; i < elements_added; i++) {
		buffer_push(&buffer, &test_values_uint16[i]);
	}

	TEST_ASSERT_EQUAL_UINT(BUFFER_MAX_ELEMENTS, buffer.element_count);
	if (mode == BUFFER_FIXED) {
		TEST_ASSERT_EQUAL_UINT(test_values_uint16[0], buffer_uint16_at(&buffer, 0));
		TEST_ASSERT_EQUAL_UINT(test_values_uint16[9], buffer_uint16_at(&buffer, 9));
	} else {
		TEST_ASSERT_EQUAL_UINT(test_values_uint16[5], buffer_uint16_at(&buffer, 0));
		TEST_ASSERT_EQUAL_UINT(test_values_uint16[14], buffer_uint16_at(&buffer, 9));
	}
}

void test_unused_elements_are_zero() {
	struct buffer buffer;
	uint8_t buffer_data[BUFFER_MAX_ELEMENTS];
	buffer_init(&buffer, buffer_data, BUFFER_MAX_ELEMENTS, sizeof(uint8_t), BUFFER_FIXED);
	uint8_t elements_added = 5;

	for (uint8_t i = 0; i < elements_added; i++) {
		buffer_push(&buffer, &test_values_uint8[i]);
	}

	for (uint8_t i = 5; i < BUFFER_MAX_ELEMENTS; i++) {
		TEST_ASSERT_EQUAL_UINT(0, buffer_uint8_at(&buffer, i));
	}		
}


