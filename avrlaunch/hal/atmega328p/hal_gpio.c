#include <math.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/log.h"
#include "avrlaunch/pgmspace/pgm_strings.h"
#include "avrlaunch/hal/hal_gpio.h"

event_descriptor gpio_to_descriptor(const gpio* gpio) {
  uint16_t port_address = (uint16_t) gpio->data;
  return (port_address << 8) | gpio->pin;
}

gpio gpio_from_descriptor(event_descriptor descriptor) {
  // Relies on the relative memory addresses of the IO registers
  // on the 328p.  Bad, mmkay.
  uint8_t* port_address = (uint8_t*) (descriptor >> 8);
  return (struct gpio) {
    .input = port_address - 2,
    .direction = port_address - 1,
    .data = port_address,
    .pin = descriptor & 0xff
  };
}

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

void gpio_set_duty_cycle_percentage(const gpio_timer* gpio_timer, uint8_t percentage) {
  // Assumes 8 bit PWM, and that compare match is in inverting mode
  uint8_t duty_cycle = 255 - round((percentage * 255) / 100.0);
  *gpio_timer->output_compare_low = duty_cycle;
}

