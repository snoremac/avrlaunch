#include <stdlib.h>
#include <string.h>

#include "unity.h"

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/buffer/buffer.h"

#include "avrlaunch/test/buffer_test.h"

#define BUFFER_MAX_ELEMENTS 10

struct thing {
	double id;
	char id_str[10];
};

static struct thing to_thing(uint8_t id);
static struct thing empty_thing(void);

void buffer_test_set_up() {}

void buffer_test_tear_down() {}

void test_push() {
	volatile struct thing buffer_data[BUFFER_MAX_ELEMENTS];
	struct buffer buffer = buffer_init(buffer_data, BUFFER_MAX_ELEMENTS, sizeof(struct thing));
	uint8_t elements_added = 5;

	for (uint8_t i = 0; i < elements_added; i++) {
		struct thing thing = to_thing(i);
		buffer_push_no_overflow(&buffer, &thing);
	}

	TEST_ASSERT_EQUAL_UINT(elements_added, buffer.element_count);
	for (uint8_t i = 0; i < elements_added; i++) {
		struct thing thing = to_thing(i);
		TEST_ASSERT_EQUAL_MEMORY(&thing, buffer_at(&buffer, i), sizeof(struct thing));
	}
}

void test_push_overflow() {
	volatile struct thing buffer_data[BUFFER_MAX_ELEMENTS];
	struct buffer buffer = buffer_init(buffer_data, BUFFER_MAX_ELEMENTS, sizeof(struct thing));
	uint8_t elements_added = 20;

	for (uint8_t i = 0; i < elements_added; i++) {
		struct thing thing = to_thing(i);
		buffer_push_overflow(&buffer, &thing);
	}

	TEST_ASSERT_EQUAL_UINT(BUFFER_MAX_ELEMENTS, buffer.element_count);
	struct thing first_thing = to_thing(10);
	struct thing last_thing = to_thing(19);
	TEST_ASSERT_EQUAL_MEMORY(&first_thing, buffer_at(&buffer, 0), sizeof(struct thing));
	TEST_ASSERT_EQUAL_MEMORY(&last_thing, buffer_at(&buffer, 9), sizeof(struct thing));
}

void test_push_no_overflow() {
	volatile struct thing buffer_data[BUFFER_MAX_ELEMENTS];
	struct buffer buffer = buffer_init(buffer_data, BUFFER_MAX_ELEMENTS, sizeof(struct thing));
	uint8_t elements_added = 20;

	for (uint8_t i = 0; i < elements_added; i++) {
		struct thing thing = to_thing(i);
		buffer_push_no_overflow(&buffer, &thing);
	}

	TEST_ASSERT_EQUAL_UINT(BUFFER_MAX_ELEMENTS, buffer.element_count);
	struct thing first_thing = to_thing(0);
	struct thing last_thing = to_thing(9);
	TEST_ASSERT_EQUAL_MEMORY(&first_thing, buffer_at(&buffer, 0), sizeof(struct thing));
	TEST_ASSERT_EQUAL_MEMORY(&last_thing, buffer_at(&buffer, 9), sizeof(struct thing));
}

void test_no_overflow_shift() {
	volatile struct thing buffer_data[BUFFER_MAX_ELEMENTS];
	struct buffer buffer = buffer_init(buffer_data, BUFFER_MAX_ELEMENTS, sizeof(struct thing));
	uint16_t elements_added = 10;
	uint16_t elements_shifted = 3;

	for (uint16_t i = 0; i < elements_added; i++) {
		struct thing thing = to_thing(i);
		buffer_push_no_overflow(&buffer, &thing);
	}

	struct thing shifts[elements_shifted];
	for (uint16_t i = 0; i < elements_shifted; i++) {
		shifts[i] = *((struct thing*) buffer_shift(&buffer));
	}

	TEST_ASSERT_EQUAL_UINT(elements_added - elements_shifted, buffer.element_count);
	for (uint16_t i = 0; i < elements_shifted; i++) {
		struct thing thing = to_thing(i);
		TEST_ASSERT_EQUAL_MEMORY(&thing, &shifts[i], sizeof(struct thing));
	}
	for (uint16_t i = 0; i < elements_added - elements_shifted; i++) {
		struct thing thing = to_thing(i + elements_shifted);
		TEST_ASSERT_EQUAL_MEMORY(&thing, buffer_at(&buffer, i), sizeof(struct thing));
	}

}

void test_unused_elements_are_zero() {
	volatile struct thing buffer_data[BUFFER_MAX_ELEMENTS];
	struct buffer buffer = buffer_init(buffer_data, BUFFER_MAX_ELEMENTS, sizeof(struct thing));
	uint8_t elements_added = 5;

	for (uint8_t i = 0; i < elements_added; i++) {
		struct thing thing = to_thing(i);
		buffer_push_no_overflow(&buffer, &thing);
	}

	for (uint8_t i = 5; i < BUFFER_MAX_ELEMENTS; i++) {
		struct thing thing = empty_thing();
		TEST_ASSERT_EQUAL_MEMORY(&thing, buffer_at(&buffer, i), sizeof(struct thing));
	}
}

static struct thing to_thing(uint8_t id) {
	struct thing thing;
	memset(&thing, 0, sizeof(struct thing));
	thing.id = id;
	dtostrf(thing.id, 3, 1, thing.id_str);
	return thing;
}

static struct thing empty_thing(void) {
	return (struct thing) { 0, "\0" };
}
