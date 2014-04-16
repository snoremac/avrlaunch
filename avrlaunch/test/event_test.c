#include <stdlib.h>

#include "unity.h"

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/clock.h"
#include "avrlaunch/event/event.h"
#include "avrlaunch/event/event_internal.h"

#include "avrlaunch/test/event_test.h"

#define EVENT_CATEGORY_TEST 255

#define EVENT_ADDRESS_TEST_A 1
#define EVENT_ADDRESS_TEST_B 2

static event* event_poll_handler_a(event_descriptor* descriptor);
static event* event_poll_handler_b(event_descriptor* descriptor);

static bool noop_event_handler(event* event);
static bool counting_event_handler(event* event);
static bool cancelling_event_handler(event* event);

static event_descriptor descriptor_a;
static event_descriptor descriptor_b;
static uint8_t poll_count_a;
static uint8_t callback_count_a;
static uint8_t callback_count_b;
static test_event current_test_event;
static test_event last_received_event;

void event_test_set_up() {
  descriptor_a = (event_descriptor) { EVENT_CATEGORY_TEST, EVENT_ADDRESS_TEST_A };
  descriptor_b = (event_descriptor) { EVENT_CATEGORY_TEST, EVENT_ADDRESS_TEST_B };
  event_clear_sources();
  event_clear_listeners();
  poll_count_a = 0;
  callback_count_a = 0;
}

void event_test_tear_down() {
}

void test_should_register_event_source() {
	event_register_source(descriptor_a, EVENT_POLL_INTERVAL_ASAP, event_poll_handler_a);
	TEST_ASSERT_EQUAL_UINT(1, event_source_count());
  TEST_ASSERT_EQUAL_UINT(descriptor_a.category, first_event_source()->super.descriptor.category);
  TEST_ASSERT_EQUAL_UINT(descriptor_a.address, first_event_source()->super.descriptor.address);
	TEST_ASSERT_EQUAL_PTR(event_poll_handler_a, first_event_source()->poll_handler);
}

void test_should_update_event_source() {
  event_register_source(descriptor_a, EVENT_POLL_INTERVAL_ASAP, event_poll_handler_a);
	TEST_ASSERT_EQUAL_UINT(1, event_source_count());

  event_register_source(descriptor_a, EVENT_POLL_INTERVAL_ASAP, event_poll_handler_b);
	TEST_ASSERT_EQUAL_UINT(1, event_source_count());
	TEST_ASSERT_EQUAL_PTR(event_poll_handler_b, first_event_source()->poll_handler);
}

void test_should_add_event_listener() {
	event_add_listener(descriptor_a, noop_event_handler);
	TEST_ASSERT_EQUAL_UINT(1, event_listener_count());
  TEST_ASSERT_EQUAL_UINT(descriptor_a.category, first_event_listener()->super.descriptor.category);
  TEST_ASSERT_EQUAL_UINT(descriptor_a.address, first_event_listener()->super.descriptor.address);
	TEST_ASSERT_EQUAL_PTR(noop_event_handler, first_event_listener()->handler);
}

void test_should_deregister_event_source() {
	event_register_source(descriptor_a, EVENT_POLL_INTERVAL_ASAP, event_poll_handler_a);
	event_register_source(descriptor_b, EVENT_POLL_INTERVAL_ASAP, event_poll_handler_b);
	TEST_ASSERT_EQUAL_UINT(2, event_source_count());

	event_deregister_source(descriptor_a);
	TEST_ASSERT_EQUAL_UINT(1, event_source_count());
  TEST_ASSERT_EQUAL_UINT(descriptor_b.category, first_event_source()->super.descriptor.category);
  TEST_ASSERT_EQUAL_UINT(descriptor_b.address, first_event_source()->super.descriptor.address);
}

void test_should_remove_event_listeners() {
	event_add_listener(descriptor_a, noop_event_handler);
	event_add_listener(descriptor_a, counting_event_handler);
	event_add_listener(descriptor_b, noop_event_handler);
	TEST_ASSERT_EQUAL_UINT(3, event_listener_count());

	event_remove_listeners(descriptor_a);
	TEST_ASSERT_EQUAL_UINT(1, event_listener_count());
  TEST_ASSERT_EQUAL_UINT(descriptor_b.category, first_event_listener()->super.descriptor.category);
  TEST_ASSERT_EQUAL_UINT(descriptor_b.address, first_event_listener()->super.descriptor.address);
}

void test_should_poll_on_specified_interval() {
	time start_time = clock_get_time();
  event_register_source(descriptor_a, 1000, event_poll_handler_a);
  event_add_listener(descriptor_a, noop_event_handler);

	event_tick();
	TEST_ASSERT_EQUAL_UINT(0, poll_count_a);

	clock_set_time(start_time + 1000);
	event_tick();
	TEST_ASSERT_EQUAL_UINT(1, poll_count_a);
}

void test_should_invoke_one_event_listener() {
  event_register_source(descriptor_a, EVENT_POLL_INTERVAL_ASAP, event_poll_handler_a);
  event_add_listener(descriptor_a, counting_event_handler);
  event_add_listener(descriptor_b, counting_event_handler);

	event_tick();
	TEST_ASSERT_EQUAL_UINT(1, callback_count_a);
	TEST_ASSERT_EQUAL_UINT(0, callback_count_b);
	TEST_ASSERT_EQUAL_UINT(44, last_received_event.value);
}

void test_should_invoke_multiple_event_listeners_for_one_source() {
  event_register_source(descriptor_a, EVENT_POLL_INTERVAL_ASAP, event_poll_handler_a);
  event_add_listener(descriptor_a, counting_event_handler);
  event_add_listener(descriptor_a, counting_event_handler);

	event_tick();
	TEST_ASSERT_EQUAL_UINT(2, callback_count_a);
}

void test_should_invoke_event_listener_repeatedly() {
  event_register_source(descriptor_a, EVENT_POLL_INTERVAL_ASAP, event_poll_handler_a);
  event_add_listener(descriptor_a, counting_event_handler);

	for (uint8_t i = 0; i < 200; i++) {
		event_tick();
	}
	TEST_ASSERT_EQUAL_UINT(200, callback_count_a);
}

void test_should_invoke_event_listeners_for_fired_event() {
  event_add_listener(descriptor_a, counting_event_handler);
  event_add_listener(descriptor_a, counting_event_handler);

	event* event = event_poll_handler_a(&descriptor_a);
  event_fire_event(event);
	TEST_ASSERT_EQUAL_UINT(2, callback_count_a);
}

void test_should_allow_event_listeners_to_remove_themselves() {
  event_register_source(descriptor_a, EVENT_POLL_INTERVAL_ASAP, event_poll_handler_a);
  event_add_listener(descriptor_a, cancelling_event_handler);

	for (uint8_t i = 0; i < 10; i++) {
		event_tick();
	}
	TEST_ASSERT_EQUAL_UINT(5, callback_count_a);
}

static event* event_poll_handler_a(event_descriptor* descriptor) {
  poll_count_a++;
	current_test_event.super.descriptor = *descriptor;
	current_test_event.value = 44;
	return (event*) &current_test_event;
}

static event* event_poll_handler_b(event_descriptor* descriptor) {
  current_test_event.super.descriptor = *descriptor;
	current_test_event.value = 59;
	return (event*) &current_test_event;
}

static bool noop_event_handler(event* event) {
	return true;
}

static bool counting_event_handler(event* event) {
	last_received_event = *((test_event*) event);
	if (event->descriptor.address == EVENT_ADDRESS_TEST_A) {
		callback_count_a++;
	} else if (event->descriptor.address == EVENT_ADDRESS_TEST_B) {
		callback_count_b++;
	}
	return true;
}

static bool cancelling_event_handler(event* event) {
	if (event->descriptor.address == EVENT_ADDRESS_TEST_A) {
		callback_count_a++;
	}
	if (callback_count_a == 5) {
		return false;
	} else {
		return true;
	}
}
