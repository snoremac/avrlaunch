#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <avr/pgmspace.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/gpio.h"
#include "avrlaunch/log.h"
#include "avrlaunch/pgmspace/pgm_strings.h"
#include "avrlaunch/hal/hal_gpio.h"

static bool is_gpio_timer_inverting(const gpio_timer* gpio_timer);

void gpio_set_input(const gpio* gpio) {
  *(gpio->direction) &= ~_BV(gpio->pin);
}

void gpio_set_output(const gpio* gpio) {
  *gpio->direction |= _BV(gpio->pin);      
}

logic_level gpio_read(const gpio* gpio) {
  return read_bit(gpio->input, gpio->pin);
}

void gpio_write(const struct gpio* gpio, logic_level level) {
  if (level == LOGIC_HIGH) {
    gpio_write_high(gpio);
  } else {
    gpio_write_low(gpio);
  }
}

void gpio_write_low(const gpio* gpio) {
  *gpio->data &= ~_BV(gpio->pin);
}

void gpio_write_high(const gpio* gpio) {
  *gpio->data |= _BV(gpio->pin);
}

void gpio_toggle(const gpio* gpio) {
  *gpio->data ^= _BV(gpio->pin);
}

void gpio_shift_out(const gpio* data_gpio, const gpio* clock_gpio, uint8_t byte) {
	for (uint8_t i = 0; i < 8; i++)  {
		gpio_write(data_gpio, !!(byte & (1 << (7 - i))));

		gpio_write(clock_gpio, LOGIC_HIGH);
		gpio_write(clock_gpio, LOGIC_LOW);
	}
}

void gpio_connect_timer_non_inverting(const gpio_timer* gpio_timer) {
  gpio_disconnect_timer(gpio_timer);
  *gpio_timer->timer_control |= gpio_timer->output_compare_non_inverting;
}

void gpio_connect_timer_inverting(const gpio_timer* gpio_timer) {
  gpio_disconnect_timer(gpio_timer);
  *gpio_timer->timer_control |= gpio_timer->output_compare_inverting;
}

void gpio_disconnect_timer(const gpio_timer* gpio_timer) {
  *gpio_timer->timer_control &= gpio_timer->output_compare_off;
}

// Assumes 8 bit PWM
void gpio_set_output_compare(const gpio_timer* gpio_timer, uint8_t output_compare) {
  if (is_gpio_timer_inverting(gpio_timer)) {
    output_compare = 255 - output_compare;
  }
  *gpio_timer->output_compare_low = output_compare;
}

void gpio_set_output_compare_percentage(const gpio_timer* gpio_timer, uint8_t output_compare_percentage) {
  gpio_set_output_compare(gpio_timer, round((output_compare_percentage * 255) / 100.0));
}

static bool is_gpio_timer_inverting(const gpio_timer* gpio_timer) {
  return (*gpio_timer->timer_control & ~gpio_timer->output_compare_off) == gpio_timer->output_compare_inverting;
}
