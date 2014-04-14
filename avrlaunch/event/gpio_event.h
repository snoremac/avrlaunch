#ifndef AVRLAUNCH_EVENT_GPIO_EVENT_H
#define AVRLAUNCH_EVENT_GPIO_EVENT_H

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/event/event.h"
#include "avrlaunch/hal/hal_gpio.h"

#define EVENT_TYPE_GPIO 1

struct gpio_read {
	logic_level current_state;
	logic_level previous_state;
};

typedef enum gpio_event_type {
	GPIO_DOWN, GPIO_UP
} gpio_event_type;

typedef struct gpio_event {
	event super;
	gpio gpio;
	gpio_event_type event_type;
} gpio_event;

struct gpio_read gpio_event_read(const gpio* gpio);

void gpio_event_add_listener(gpio* gpio, event_handler handler);

void gpio_event_remove_listeners(gpio* gpio);

event_descriptor gpio_to_descriptor(const gpio* gpio);

struct gpio gpio_from_descriptor(event_descriptor);

#endif
