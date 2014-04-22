#include <stdlib.h>

#include "unity.h"

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/clock.h"
#include "avrlaunch/log.h"
#include "avrlaunch/event/event.h"
#include "avrlaunch/event/event_internal.h"
#include "avrlaunch/event/gpio_event.h"

#include "avrlaunch/test/gpio_event_test.h"

static event last_event;
static uint8_t callback_count;

static bool gpio_event_handler(event* event);
static bool gpios_equal(gpio* gpio_a, gpio* gpio_b);

void gpio_event_test_set_up() {
	event_clear_sources();
	event_clear_listeners();
	callback_count = 0;
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

void test_should_raise_event_after_logic_level_change() {
	struct gpio gpio = GPIO(PORTD, PIN2);
	gpio_set_output(&gpio);
	gpio_write_low(&gpio);
	gpio_set_input(&gpio);
	
	gpio_event_add_listener(&gpio, gpio_event_handler);
	gpio_write_high(&gpio);
	//LOG_DEBUG("Time: %lu\n", clock_get_time());
	delay(50);
	//LOG_DEBUG("Time: %lu\n", clock_get_time());
	event_tick();

	TEST_ASSERT_EQUAL_UINT(1, callback_count);
	TEST_ASSERT_TRUE(gpio_event_is_from(&last_event, &gpio));
	TEST_ASSERT_TRUE(gpio_event_is_up(&last_event));
}

void test_not_should_raise_without_logic_level_change() {
	struct gpio gpio = GPIO(PORTD, PIN2);
	gpio_set_input(&gpio);
	gpio_write_high(&gpio);

	gpio_event_add_listener(&gpio, gpio_event_handler);
	delay(50);
	event_tick();
	delay(50);
	event_tick();

	TEST_ASSERT_EQUAL_UINT(0, callback_count);
}

void test_gpio_descriptor(void) {
	struct gpio gpio_a = GPIO(PORTD, PIN2);
	event_descriptor descriptor = gpio_to_descriptor(&gpio_a);
	struct gpio gpio_b = gpio_from_descriptor(&descriptor);
	TEST_ASSERT_TRUE(gpios_equal(&gpio_a, &gpio_b));
}

static bool gpio_event_handler(event* event) {
	callback_count++;
	last_event = *event;
	return true;
}

static bool gpios_equal(gpio* gpio_a, gpio* gpio_b) {
	return gpio_a != NULL
			&& gpio_b != NULL
			&& gpio_a->index == gpio_b->index
			&& gpio_a->input == gpio_b->input
			&& gpio_a->direction == gpio_b->direction
			&& gpio_a->data == gpio_b->data
			&& gpio_a->pin == gpio_b->pin;
}
