#ifndef AVRLAUNCH_RGB_LED_H
#define AVRLAUNCH_RGB_LED_H

#include "avrlaunch/gpio.h"
#include "avrlaunch/hal/hal_gpio.h"

#define RGB(RED, GREEN, BLUE) (struct rgb) { .red = RED, .green = GREEN, .blue = BLUE }

#define RGB_RED RGB(255, 0, 0)
#define RGB_GREEN RGB(0, 255, 0)
#define RGB_YELLOW RGB(255, 128, 0)

struct rgb {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

struct rgb_led {
	const gpio_timer* red_gpio_timer;
	const gpio_timer* green_gpio_timer;
	const gpio_timer* blue_gpio_timer;
};

void rgb_led_set_color(const struct rgb_led* led, const struct rgb color);

void rgb_led_off(const struct rgb_led* led);

#endif