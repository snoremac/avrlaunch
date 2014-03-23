#include <stdlib.h>

#include "unity.h"

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/clock.h"
#include "avrlaunch/event/event.h"
#include "avrlaunch/event/event_internal.h"

#include "avrlaunch/test/event_test.h"

#define EVENT_TYPE_TEST 0xFF

#define EVENT_DESCRIPTOR_TEST_A 0x01
#define EVENT_DESCRIPTOR_TEST_B 0x02

static event* poll_event_source_a(event_type type, event_descriptor descriptor);
static event* poll_event_source_b(event_type type, event_descriptor descriptor);

static bool noop_event_handler(event* event);
static bool counting_event_handler(event* event);
static bool cancelling_event_handler(event* event);

static uint8_t poll_count_a;
static uint8_t callback_count_a;
static uint8_t callback_count_b;
static test_event current_test_event;
static test_event last_received_event;

void event_test_set_up() {
  event_clear_sources();
  event_clear_listeners();
  poll_count_a = 0;
  callback_count_a = 0;
}

void event_test_tear_down() {
}

void test_should_register_event_source() {
	result result = event_register_source(EVENT_TYPE_TEST, EVENT_DESCRIPTOR_TEST_A, EVENT_ASAP, poll_event_source_a);
	TEST_ASSERT_EQUAL_UINT(RESULT_SUCCESS, result);
	TEST_ASSERT_EQUAL_UINT(1, event_source_count());
	TEST_ASSERT_EQUAL_UINT(EVENT_TYPE_TEST, ((event_source*) first_event_source())->super.type);
	TEST_ASSERT_EQUAL_UINT(EVENT_DESCRIPTOR_TEST_A, ((event_source*) first_event_source())->super.descriptor);
	TEST_ASSERT_EQUAL_PTR(poll_event_source_a, ((event_source*) first_event_source())->poll_handler);
}

void test_should_update_event_source() {
	result result = event_register_source(EVENT_TYPE_TEST, EVENT_DESCRIPTOR_TEST_A, EVENT_ASAP, poll_event_source_a);
	TEST_ASSERT_EQUAL_UINT(RESULT_SUCCESS, result);
	TEST_ASSERT_EQUAL_UINT(1, event_source_count());

	result = event_register_source(EVENT_TYPE_TEST, EVENT_DESCRIPTOR_TEST_A, EVENT_ASAP, poll_event_source_b);
	TEST_ASSERT_EQUAL_UINT(RESULT_SUCCESS, result);
	TEST_ASSERT_EQUAL_UINT(1, event_source_count());
	TEST_ASSERT_EQUAL_PTR(poll_event_source_b, ((event_source*) first_event_source())->poll_handler);
}

void test_should_add_event_listener() {
	result result = event_add_listener(EVENT_TYPE_TEST, EVENT_DESCRIPTOR_TEST_A, noop_event_handler);
	TEST_ASSERT_EQUAL_UINT(RESULT_SUCCESS, result);
	TEST_ASSERT_EQUAL_UINT(1, event_listener_count());
	TEST_ASSERT_EQUAL_UINT(EVENT_TYPE_TEST, ((event_listener*) first_event_listener())->super.type);
	TEST_ASSERT_EQUAL_UINT(EVENT_DESCRIPTOR_TEST_A, ((event_listener*) first_event_listener())->super.descriptor);
	TEST_ASSERT_EQUAL_PTR(noop_event_handler, ((event_listener*) first_event_listener())->handler);
}

void test_should_deregister_event_source() {
	event_register_source(EVENT_TYPE_TEST, EVENT_DESCRIPTOR_TEST_A, EVENT_ASAP, poll_event_source_a);
	event_register_source(EVENT_TYPE_TEST, EVENT_DESCRIPTOR_TEST_B, EVENT_ASAP, poll_event_source_b);
	TEST_ASSERT_EQUAL_UINT(2, event_source_count());

	event_deregister_source(EVENT_TYPE_TEST, EVENT_DESCRIPTOR_TEST_A);
	TEST_ASSERT_EQUAL_UINT(1, event_source_count());
	TEST_ASSERT_EQUAL_UINT(EVENT_DESCRIPTOR_TEST_B, ((event_source*) first_event_source())->super.descriptor);
}

void test_should_remove_event_listeners() {
	event_add_listener(EVENT_TYPE_TEST, EVENT_DESCRIPTOR_TEST_A, noop_event_handler);
	event_add_listener(EVENT_TYPE_TEST, EVENT_DESCRIPTOR_TEST_A, counting_event_handler);
	event_add_listener(EVENT_TYPE_TEST, EVENT_DESCRIPTOR_TEST_B, noop_event_handler);
	TEST_ASSERT_EQUAL_UINT(3, event_listener_count());

	event_remove_listeners(EVENT_TYPE_TEST, EVENT_DESCRIPTOR_TEST_A);
	TEST_ASSERT_EQUAL_UINT(1, event_listener_count());
	TEST_ASSERT_EQUAL_UINT(EVENT_DESCRIPTOR_TEST_B, ((event_listener*) first_event_listener())->super.descriptor);
}

void test_should_poll_on_specified_interval() {
	time start_time = clock_get_time();
	event_register_source(EVENT_TYPE_TEST, EVENT_DESCRIPTOR_TEST_A, 1000, poll_event_source_a);
	event_add_listener(EVENT_TYPE_TEST, EVENT_DESCRIPTOR_TEST_A, noop_event_handler);	

	event_tick();
	TEST_ASSERT_EQUAL_UINT(0, poll_count_a);

	clock_set_time(start_time + 1000);
	event_tick();
	TEST_ASSERT_EQUAL_UINT(1, poll_count_a);
}

void test_should_invoke_one_event_listener() {
	event_register_source(EVENT_TYPE_TEST, EVENT_DESCRIPTOR_TEST_A, EVENT_ASAP, poll_event_source_a);
	event_add_listener(EVENT_TYPE_TEST, EVENT_DESCRIPTOR_TEST_A, counting_event_handler);	
	event_add_listener(EVENT_TYPE_TEST, EVENT_DESCRIPTOR_TEST_B, counting_event_handler);	

	event_tick();
	TEST_ASSERT_EQUAL_UINT(1, callback_count_a);
	TEST_ASSERT_EQUAL_UINT(0, callback_count_b);
	TEST_ASSERT_EQUAL_UINT(44, last_received_event.value);
}

void test_should_invoke_multiple_event_listeners_for_one_source() {
	event_register_source(EVENT_TYPE_TEST, EVENT_DESCRIPTOR_TEST_A, EVENT_ASAP, poll_event_source_a);
	event_add_listener(EVENT_TYPE_TEST, EVENT_DESCRIPTOR_TEST_A, counting_event_handler);	
	event_add_listener(EVENT_TYPE_TEST, EVENT_DESCRIPTOR_TEST_A, counting_event_handler);	

	event_tick();
	TEST_ASSERT_EQUAL_UINT(2, callback_count_a);
}

void test_should_invoke_event_listener_repeatedly() {
	event_register_source(EVENT_TYPE_TEST, EVENT_DESCRIPTOR_TEST_A, EVENT_ASAP, poll_event_source_a);
	event_add_listener(EVENT_TYPE_TEST, EVENT_DESCRIPTOR_TEST_A, counting_event_handler);	

	for (uint8_t i = 0; i < 200; i++) {
		event_tick();		
	}
	TEST_ASSERT_EQUAL_UINT(200, callback_count_a);	
}

void test_should_invoke_event_listeners_for_fired_event() {
	event_add_listener(EVENT_TYPE_TEST, EVENT_DESCRIPTOR_TEST_A, counting_event_handler);	
	event_add_listener(EVENT_TYPE_TEST, EVENT_DESCRIPTOR_TEST_A, counting_event_handler);	

	event* event = poll_event_source_a(EVENT_TYPE_TEST, EVENT_DESCRIPTOR_TEST_A);
  event_fire_event(event);
	TEST_ASSERT_EQUAL_UINT(2, callback_count_a);
}

void test_should_allow_event_listeners_to_remove_themselves() {
	event_register_source(EVENT_TYPE_TEST, EVENT_DESCRIPTOR_TEST_A, EVENT_ASAP, poll_event_source_a);
	event_add_listener(EVENT_TYPE_TEST, EVENT_DESCRIPTOR_TEST_A, cancelling_event_handler);

	for (uint8_t i = 0; i < 10; i++) {
		event_tick();		
	}
	TEST_ASSERT_EQUAL_UINT(5, callback_count_a);		
}

static event* poll_event_source_a(event_type type, event_descriptor descriptor) {
  poll_count_a++;
	current_test_event.super.type = EVENT_TYPE_TEST;
	current_test_event.super.descriptor = EVENT_DESCRIPTOR_TEST_A;
	current_test_event.value = 44;
	return (event*) &current_test_event;
}

static event* poll_event_source_b(event_type type, event_descriptor descriptor) {
	current_test_event.super.type = EVENT_TYPE_TEST;
	current_test_event.super.descriptor = EVENT_DESCRIPTOR_TEST_B;
	current_test_event.value = 59;
	return (event*) &current_test_event;
}

static bool noop_event_handler(event* event) {
	return true;
}

static bool counting_event_handler(event* event) {
	last_received_event = *((test_event*) event);
	if (event->descriptor == EVENT_DESCRIPTOR_TEST_A) {
		callback_count_a++;		
	} else if (event->descriptor == EVENT_DESCRIPTOR_TEST_B) {
		callback_count_b++;
	}		
	return true;
}

static bool cancelling_event_handler(event* event) {
	if (event->descriptor == EVENT_DESCRIPTOR_TEST_A) {
		callback_count_a++;
	}
	if (callback_count_a == 5) {
		return false;
	} else {
		return true;
	}
}