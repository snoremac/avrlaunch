#include <stdlib.h>
#include <string.h>

#include "unity.h"

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/buffer/buffer_int.h"

#include "avrlaunch/test/buffer_int_test.h"

#define BUFFER_MAX_ELEMENTS 10

static uint8_t test_values_uint8[15] = {
	43, 213, 119, 80, 2, 78, 200, 189, 14, 167, 101, 32, 9, 229, 67
};

static uint16_t test_values_uint16[15] = {
	6237, 23890, 23, 931, 61001, 278, 448, 11987, 1089, 2299, 467, 1730, 48500, 39899, 7
};

void buffer_int_test_set_up() {}

void buffer_int_test_tear_down() {}

void test_push_uint8() {
	volatile uint8_t buffer_data[BUFFER_MAX_ELEMENTS];
	struct buffer buffer = buffer_init(buffer_data, BUFFER_MAX_ELEMENTS, sizeof(uint8_t));
	uint8_t elements_added = 5;

	for (uint8_t i = 0; i < elements_added; i++) {
		buffer_push_no_overflow(&buffer, &test_values_uint8[i]);
	}

	TEST_ASSERT_EQUAL_UINT(elements_added, buffer.element_count);
	for (uint8_t i = 0; i < elements_added; i++) {
		TEST_ASSERT_EQUAL_UINT(test_values_uint8[i], buffer_uint8_at(&buffer, i));
	}
}

void test_push_overflow_uint8() {
	volatile uint8_t buffer_data[BUFFER_MAX_ELEMENTS];
	struct buffer buffer = buffer_init(buffer_data, BUFFER_MAX_ELEMENTS, sizeof(uint8_t));
	uint8_t elements_added = 15;

	for(int i = 0; i < elements_added; i++) {
		buffer_push_overflow(&buffer, &test_values_uint8[i]);
	}

	TEST_ASSERT_EQUAL_UINT(BUFFER_MAX_ELEMENTS, buffer.element_count);
	TEST_ASSERT_EQUAL_UINT(test_values_uint8[5], buffer_uint8_at(&buffer, 0));
	TEST_ASSERT_EQUAL_UINT(test_values_uint8[14], buffer_uint8_at(&buffer, 9));
}

void test_push_no_overflow_uint8() {
	volatile uint8_t buffer_data[BUFFER_MAX_ELEMENTS];
	struct buffer buffer = buffer_init(buffer_data, BUFFER_MAX_ELEMENTS, sizeof(uint8_t));
	uint8_t elements_added = 15;

	for(int i = 0; i < elements_added; i++) {
		buffer_push_no_overflow(&buffer, &test_values_uint8[i]);
	}

	TEST_ASSERT_EQUAL_UINT(BUFFER_MAX_ELEMENTS, buffer.element_count);
	TEST_ASSERT_EQUAL_UINT(test_values_uint8[0], buffer_uint8_at(&buffer, 0));
	TEST_ASSERT_EQUAL_UINT(test_values_uint8[9], buffer_uint8_at(&buffer, 9));
}

void test_no_overflow_shift_uint8() {
	volatile uint8_t buffer_data[BUFFER_MAX_ELEMENTS];
	struct buffer buffer = buffer_init(buffer_data, BUFFER_MAX_ELEMENTS, sizeof(uint8_t));
	uint8_t elements_added = 10;
	uint8_t elements_shifted = 3;

	for (uint8_t i = 0; i < elements_added; i++) {
		buffer_push_no_overflow(&buffer, &test_values_uint8[i]);
	}

	uint8_t shifts[elements_shifted];
	for (uint8_t i = 0; i < elements_shifted; i++) {
		shifts[i] = buffer_shift_uint8(&buffer);
	}

	TEST_ASSERT_EQUAL_UINT(elements_added - elements_shifted, buffer.element_count);
	for (uint8_t i = 0; i < elements_shifted; i++) {
		TEST_ASSERT_EQUAL_UINT(shifts[i], test_values_uint8[i]);
	}
	for (uint8_t i = 0; i < elements_added - elements_shifted; i++) {
		TEST_ASSERT_EQUAL_UINT(test_values_uint8[i + elements_shifted], buffer_uint8_at(&buffer, i));
	}
}

void test_push_uint16() {
	volatile uint16_t buffer_data[BUFFER_MAX_ELEMENTS];
	struct buffer buffer = buffer_init(buffer_data, BUFFER_MAX_ELEMENTS, sizeof(uint16_t));
	uint16_t elements_added = 5;

	for (uint16_t i = 0; i < elements_added; i++) {
		buffer_push_no_overflow(&buffer, &test_values_uint16[i]);
	}

	TEST_ASSERT_EQUAL_UINT(elements_added, buffer.element_count);
	for (uint16_t i = 0; i < elements_added; i++) {
		TEST_ASSERT_EQUAL_UINT(test_values_uint16[i], buffer_uint16_at(&buffer, i));
	}
}

void test_push_overflow_uint16() {
	volatile uint16_t buffer_data[BUFFER_MAX_ELEMENTS];
	struct buffer buffer = buffer_init(buffer_data, BUFFER_MAX_ELEMENTS, sizeof(uint16_t));
	uint16_t elements_added = 15;

	for(int i = 0; i < elements_added; i++) {
		buffer_push_overflow(&buffer, &test_values_uint16[i]);
	}

	TEST_ASSERT_EQUAL_UINT(BUFFER_MAX_ELEMENTS, buffer.element_count);
	TEST_ASSERT_EQUAL_UINT(test_values_uint16[5], buffer_uint16_at(&buffer, 0));
	TEST_ASSERT_EQUAL_UINT(test_values_uint16[14], buffer_uint16_at(&buffer, 9));
}

void test_push_no_overflow_uint16() {
	volatile uint16_t buffer_data[BUFFER_MAX_ELEMENTS];
	struct buffer buffer = buffer_init(buffer_data, BUFFER_MAX_ELEMENTS, sizeof(uint16_t));
	uint16_t elements_added = 15;

	for(int i = 0; i < elements_added; i++) {
		buffer_push_no_overflow(&buffer, &test_values_uint16[i]);
	}

	TEST_ASSERT_EQUAL_UINT(BUFFER_MAX_ELEMENTS, buffer.element_count);
	TEST_ASSERT_EQUAL_UINT(test_values_uint16[0], buffer_uint16_at(&buffer, 0));
	TEST_ASSERT_EQUAL_UINT(test_values_uint16[9], buffer_uint16_at(&buffer, 9));
}

void test_no_overflow_shift_uint16() {
	volatile uint16_t buffer_data[BUFFER_MAX_ELEMENTS];
	struct buffer buffer = buffer_init(buffer_data, BUFFER_MAX_ELEMENTS, sizeof(uint16_t));
	uint16_t elements_added = 10;
	uint16_t elements_shifted = 3;

	for (uint16_t i = 0; i < elements_added; i++) {
		buffer_push_no_overflow(&buffer, &test_values_uint16[i]);
	}

	uint16_t shifts[elements_shifted];
	for (uint16_t i = 0; i < elements_shifted; i++) {
		shifts[i] = buffer_shift_uint16(&buffer);
	}

	TEST_ASSERT_EQUAL_UINT(elements_added - elements_shifted, buffer.element_count);
	for (uint16_t i = 0; i < elements_shifted; i++) {
		TEST_ASSERT_EQUAL_UINT(shifts[i], test_values_uint16[i]);
	}
	for (uint16_t i = 0; i < elements_added - elements_shifted; i++) {
		TEST_ASSERT_EQUAL_UINT(test_values_uint16[i + elements_shifted], buffer_uint16_at(&buffer, i));
	}
}
