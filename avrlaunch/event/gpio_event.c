
#include <stdlib.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/clock.h"
#include "avrlaunch/log.h"
#include "avrlaunch/event/event.h"
#include "avrlaunch/event/gpio_event.h"
#include "avrlaunch/hal/hal_gpio.h"

static logic_level last_gpio_reads[3][8];
static event current_event;

static event* gpio_poll_handler(event_descriptor* descriptor);

struct gpio_read gpio_event_read(const gpio* gpio) {
	logic_level current_state = gpio_read(gpio);
	struct gpio_read read = {
		.current_state = current_state,
		.previous_state = last_gpio_reads[gpio->index][gpio->pin]
	};

	last_gpio_reads[gpio->index][gpio->pin] = current_state;
	return read;
}

void gpio_event_add_listener(gpio* gpio, event_handler handler) {
	gpio_event_read(gpio);
  event_register_source(gpio_to_descriptor(gpio), 50, gpio_poll_handler);
  event_add_listener(gpio_to_descriptor(gpio), handler);
}

void gpio_event_remove_listeners(gpio* gpio) {
  event_remove_listeners(gpio_to_descriptor(gpio));
}

event_descriptor gpio_to_descriptor(const gpio* gpio) {
	event_descriptor descriptor = (event_descriptor) { EVENT_CATEGORY_GPIO };
	uint16_t port_address = (uint16_t) gpio->data;
	descriptor.address = (port_address << 8) | (gpio->index << 4) | gpio->pin;
	return descriptor;
}

struct gpio gpio_from_descriptor(event_descriptor* descriptor) {
	// Relies on the relative memory addresses of the IO registers
	// on the 328p.  Bad, mmkay.
	uint8_t* port_address = (uint8_t*) (descriptor->address >> 8);
	return (struct gpio) {
		.index = (descriptor->address >> 4) & 0x0f,
		.input = port_address - 2,
		.direction = port_address - 1,
		.data = port_address,
		.pin = descriptor->address & 0x0f
	};
}

bool gpio_event_is_from(event* event, struct gpio* gpio) {
	event_descriptor descriptor = gpio_to_descriptor(gpio);
	return descriptor.category == event->descriptor.category && descriptor.address == event->descriptor.address;
}

bool gpio_event_is_up(event* event) {
	return event->flags & GPIO_UP;
}

bool gpio_event_is_down(event* event) {
	return event->flags & GPIO_DOWN;
}

static event* gpio_poll_handler(event_descriptor* descriptor) {
	gpio gpio = gpio_from_descriptor(descriptor);
	struct gpio_read read = gpio_event_read(&gpio);

	if (read.current_state == read.previous_state) return NULL;
	if (read.current_state == LOGIC_UNKNOWN) return NULL;
	if (read.previous_state == LOGIC_UNKNOWN) return NULL;

	current_event.descriptor = *descriptor;
	current_event.flags = 0x00;
	
	if (read.current_state == LOGIC_HIGH) {
		current_event.flags |= GPIO_UP;
	} else {
		current_event.flags |= GPIO_DOWN;				
	}
	return &current_event;
}
