	
#include "avrlaunch/rgb_led.h"
#include "avrlaunch/hal/hal_gpio.h"

void rgb_led_set_color(const struct rgb_led* led, const struct rgb color) {
	gpio_set_output_compare(led->red_gpio_timer, color.red);
	gpio_set_output_compare(led->green_gpio_timer, color.green);
	gpio_set_output_compare(led->blue_gpio_timer, color.blue);
}

void rgb_led_off(const struct rgb_led* led) {
	rgb_led_set_color(led, RGB(0, 0, 0));
}
