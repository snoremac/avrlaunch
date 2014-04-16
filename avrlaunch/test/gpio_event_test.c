#include <stdlib.h>

#include "unity.h"

#include "avrlaunch/clock.h"
#include "avrlaunch/event/event.h"
#include "avrlaunch/event/event_internal.h"
#include "avrlaunch/event/gpio_event.h"

#include "avrlaunch/test/gpio_event_test.h"

static gpio_event last_event;
static uint8_t callback_count;

static bool gpio_event_handler(event* event);

void gpio_event_test_set_up() {
	event_clear_sources();
	event_clear_listeners();
}

void gpio_event_test_tear_down() {
}

void test_should_register_event_source_when_event_listener_added() {
	struct gpio gpio = GPIO(PORTD, PIN2);
	gpio_event_add_listener(&gpio, gpio_event_handler);

	TEST_ASSERT_EQUAL_UINT(1, event_source_count());
	TEST_ASSERT_EQUAL_UINT(EVENT_CATEGORY_GPIO, ((event_source*) first_event_source())->super.descriptor.category);
	TEST_ASSERT_EQUAL_UINT(&PORTD,
      gpio_from_descriptor(&first_event_source()->super.descriptor).data);
	TEST_ASSERT_EQUAL_UINT(PIN2,
      gpio_from_descriptor(&first_event_source()->super.descriptor).pin);
}

void test_should_deregister_event_source_when_event_listener_removed() {
	struct gpio gpio_a = GPIO(PORTD, PIN2);
	struct gpio gpio_b = GPIO(PORTD, PIN3);
	gpio_event_add_listener(&gpio_a, gpio_event_handler);
	gpio_event_add_listener(&gpio_b, gpio_event_handler);
	TEST_ASSERT_EQUAL_UINT(2, event_source_count());

	gpio_event_remove_listeners(&gpio_a);
	TEST_ASSERT_EQUAL_UINT(1, event_source_count());
	TEST_ASSERT_EQUAL_UINT(&PORTD,
      gpio_from_descriptor(&first_event_source()->super.descriptor).data);
	TEST_ASSERT_EQUAL_UINT(PIN3,
      gpio_from_descriptor(&first_event_source()->super.descriptor).pin);

	gpio_event_remove_listeners(&gpio_b);
	TEST_ASSERT_EQUAL_UINT(0, event_source_count());
}

#ifdef BOARD_mock
void test_should_raise_event_after_logic_level_change() {
	struct gpio gpio = GPIO(PORTD, PIN2);
	gpio_set_mode(&gpio, GPIO_INPUT);
	gpio_set_current_state(&gpio, LOGIC_LOW);

	gpio_event_add_listener(&gpio, gpio_event_handler);
	
	gpio_set_current_state(&gpio, LOGIC_HIGH);
	event_tick();

	TEST_ASSERT_EQUAL_UINT(1, callback_count);
	TEST_ASSERT_EQUAL_UINT(GPIO_UP, last_event.event_type);
	TEST_ASSERT_EQUAL_UINT(&PORTD, last_event.gpio.data);
	TEST_ASSERT_EQUAL_UINT(PIN2, last_event.gpio.pin);
}

void test_not_should_raise_without_logic_level_change() {
	struct gpio gpio = GPIO(PORTD, PIN2);
	gpio_set_mode(&gpio, GPIO_INPUT);
	gpio_set_current_state(&gpio, LOGIC_LOW);

	gpio_event_add_listener(&gpio, gpio_event_handler);
	event_tick();

	TEST_ASSERT_EQUAL_UINT(0, callback_count);
}
#endif

static bool gpio_event_handler(event* event) {
	callback_count++;
	last_event = *((gpio_event*) event);
	return true;
}
