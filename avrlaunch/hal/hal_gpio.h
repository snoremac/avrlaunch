 #ifndef AVRLAUNCH_HAL_GPIO_H
#define AVRLAUNCH_HAL_GPIO_H

#include <inttypes.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/hal/hal.h"
#include HAL_MODULE(gpio)

typedef struct gpio gpio;
typedef struct gpio_timer gpio_timer;

event_descriptor gpio_to_descriptor(const gpio* gpio);

gpio gpio_from_descriptor(event_descriptor descriptor);

void gpio_set_input(const gpio* gpio) __attribute__((always_inline));

void gpio_set_output(const gpio* gpio) __attribute__((always_inline));

logic_level gpio_read(const gpio* gpio);

void gpio_write(const gpio* gpio, logic_level level);

void gpio_write_low(const gpio* gpio) __attribute__((always_inline));

void gpio_write_high(const gpio* gpio) __attribute__((always_inline));

void gpio_toggle(const gpio* gpio);

void gpio_shift_out(const gpio* data_gpio, const gpio* clock_gpio, uint8_t byte);

void gpio_connect_timer_non_inverting(const gpio_timer* gpio_timer);

void gpio_connect_timer_inverting(const gpio_timer* gpio_timer);

void gpio_disconnect_timer(const gpio_timer* gpio_timer);

void gpio_set_output_compare(const gpio_timer* gpio_timer, uint8_t output_compare);

void gpio_set_output_compare_percentage(const gpio_timer* gpio_timer, uint8_t output_compare_percentage);

#endif
