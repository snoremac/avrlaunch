#include <stdlib.h>

#include "unity.h"

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/buffer/buffer.h"
#include "avrlaunch/clock.h"
#include "avrlaunch/event/event.h"
#include "avrlaunch/event/event_internal.h"
#include "avrlaunch/event/buffer_event.h"

#include "avrlaunch/test/buffer_event_test.h"

#define BUFFER_MAX_ELEMENTS 10

static event last_event;
static uint8_t callback_count;

static bool buffer_event_handler(event* event);

void buffer_event_test_set_up() {
	event_clear_sources();
	event_clear_listeners();
	buffer_event_init();
	callback_count = 0;
}

void buffer_event_test_tear_down() {
}

void test_should_register_buffer_event_source_when_event_listener_added() {
	uint8_t buffer_data[BUFFER_MAX_ELEMENTS];
	struct buffer buffer = buffer_init(buffer_data, BUFFER_MAX_ELEMENTS, sizeof(uint8_t));
	buffer_event_add_listener(&buffer, buffer_event_handler);

	TEST_ASSERT_EQUAL_UINT(1, event_source_count());
	TEST_ASSERT_EQUAL_UINT(EVENT_CATEGORY_BUFFER, ((event_source*) first_event_source())->super.descriptor.category);
	TEST_ASSERT_EQUAL_UINT(&buffer, first_event_source()->super.descriptor.address);
}

void test_should_raise_single_event_on_buffer_full() {
	uint8_t buffer_data[BUFFER_MAX_ELEMENTS];
	struct buffer buffer = buffer_init(buffer_data, BUFFER_MAX_ELEMENTS, sizeof(uint8_t));
	buffer_event_add_listener(&buffer, buffer_event_handler);

	for (uint8_t i = 0; i < BUFFER_MAX_ELEMENTS - 1; i++) {
		buffer_push_overflow(&buffer, &i);
		event_tick();
	}
	TEST_ASSERT_EQUAL_UINT(0, callback_count);
	
	buffer_push_overflow(&buffer, &(uint8_t){ 1 });
	event_tick();
	TEST_ASSERT_EQUAL_UINT(1, callback_count);
	TEST_ASSERT_EQUAL_PTR(&buffer, last_event.descriptor.address);
	TEST_ASSERT_BITS_HIGH(BUFFER_FULL, last_event.flags);

	for (uint8_t i = 0; i < BUFFER_MAX_ELEMENTS; i++) {
		buffer_push_overflow(&buffer, &i);
		event_tick();
	}
	TEST_ASSERT_EQUAL_UINT(1, callback_count);
}

void test_should_raise_single_event_on_buffer_not_full() {
	uint8_t buffer_data[BUFFER_MAX_ELEMENTS];
	struct buffer buffer = buffer_init(buffer_data, BUFFER_MAX_ELEMENTS, sizeof(uint8_t));

	for (uint8_t i = 0; i < BUFFER_MAX_ELEMENTS; i++) {
		buffer_push_overflow(&buffer, &i);
	}
	
	buffer_event_add_listener(&buffer, buffer_event_handler);
	event_tick();
	TEST_ASSERT_EQUAL_UINT(0, callback_count);
	
	buffer_shift(&buffer);
	event_tick();
	TEST_ASSERT_EQUAL_UINT(1, callback_count);
	TEST_ASSERT_EQUAL_PTR(&buffer, last_event.descriptor.address);
	TEST_ASSERT_BITS_HIGH(BUFFER_NOT_FULL, last_event.flags);

	for (uint8_t i = 0; i < 5; i++) {
		buffer_shift(&buffer);
		event_tick();
	}
	TEST_ASSERT_EQUAL_UINT(1, callback_count);
}

static bool buffer_event_handler(event* event) {
	callback_count++;
	last_event = *event;
	return true;
}
