#include <stdlib.h>

#include "unity.h"

#include "avrlaunch/event/event.h"
#include "avrlaunch/event/event_internal.h"
#include "avrlaunch/event/adc_event.h"
#include "avrlaunch/hal/hal.h"

#include "avrlaunch/test/adc_event_test.h"

static adc_event last_event;
static uint8_t callback_count;

static bool adc_event_handler(event* event);

void adc_event_test_set_up() {
	event_clear_sources();
	event_clear_listeners();
}

void adc_event_test_tear_down() {
}

void test_should_register_event_source_when_listener_added() {
	uint8_t channel = 2;
	adc_event_add_listener(channel, 20, adc_event_handler);
	TEST_ASSERT_EQUAL_UINT(1, event_source_count());
	TEST_ASSERT_EQUAL_UINT(EVENT_CATEGORY_ADC, first_event_source()->super.descriptor.category);
	TEST_ASSERT_EQUAL_UINT(channel, first_event_source()->super.descriptor.address);
}

void test_should_deregister_event_source_when_listener_removed() {
	uint8_t channel_2 = 2;
	uint8_t channel_3 = 3;
	adc_event_add_listener(channel_2, 20, adc_event_handler);
	adc_event_add_listener(channel_3, 20, adc_event_handler);
	TEST_ASSERT_EQUAL_UINT(2, event_source_count());

	adc_event_remove_listeners(channel_2);
	TEST_ASSERT_EQUAL_UINT(1, event_source_count());
	TEST_ASSERT_EQUAL_UINT(channel_3, first_event_source()->super.descriptor.address);

	adc_event_remove_listeners(channel_3);
	TEST_ASSERT_EQUAL_UINT(0, event_source_count());
}

#ifdef BOARD_mock
static void simulate_adc_change(uint8_t channel, uint8_t amount, uint8_t threshold);

void test_should_raise_event_after_reading_changed_beyond_threshold() {
	uint8_t channel = 2;
	simulate_adc_change(channel, 20, 20);
	TEST_ASSERT_EQUAL_UINT(1, callback_count);
	TEST_ASSERT_EQUAL_UINT(channel, last_event.super.descriptor.address);
}

void test_should_not_raise_event_when_reading_changes_below_threshold() {
	uint8_t channel = 2;
	simulate_adc_change(channel, 10, 20);
	TEST_ASSERT_EQUAL_UINT(0, callback_count);
}

static void simulate_adc_change(uint8_t channel, uint8_t amount, uint8_t threshold) {
	adc_set_current_state(channel, 500);
	adc_event_add_listener(channel, threshold, adc_event_handler);
	
  clock_set_time(clock_get_time() + 100);
	adc_set_current_state(channel, 500 + amount);
	event_tick();
}
#endif

static bool adc_event_handler(event* event) {
	callback_count++;
	last_event = *((adc_event*) event);
	return true;
}
