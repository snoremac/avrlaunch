#ifndef AVRLAUNCH_EVENT_GPIO_EVENT_H
#define AVRLAUNCH_EVENT_GPIO_EVENT_H

#include <stdbool.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/gpio.h"
#include "avrlaunch/event/event.h"
#include "avrlaunch/hal/hal_gpio.h"

#define EVENT_CATEGORY_GPIO 1

#define GPIO_DOWN 0x01
#define GPIO_UP 0x02

struct gpio_read {
	logic_level current_state;
	logic_level previous_state;
};

struct gpio_read gpio_event_read(const gpio* gpio);

void gpio_event_add_listener(gpio* gpio, event_handler handler);

void gpio_event_remove_listeners(gpio* gpio);

event_descriptor gpio_to_descriptor(const gpio* gpio);

struct gpio gpio_from_descriptor(event_descriptor* descriptor);

bool gpio_event_is_from(event* event, struct gpio* gpio);

bool gpio_event_is_up(event* event);

bool gpio_event_is_down(event* event);

#endif
