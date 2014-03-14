#ifndef AVR_KIT_HAL_ATMEGA328P_HAL_GPIO_H
#define AVR_KIT_HAL_ATMEGA328P_HAL_GPIO_H

#include <inttypes.h>
#include <avr/io.h>

#include "avrlaunch/hal/hal_gpio.h"

struct gpio {
	uint8_t index;
	volatile uint8_t* input;
	volatile uint8_t* direction;
	volatile uint8_t* data;
	uint8_t pin;
};

struct gpio_timer {
	volatile uint8_t* timer_control;
	volatile uint8_t* output_compare_low;
	volatile uint8_t* output_compare_high;
	uint8_t output_compare_off;
	uint8_t output_compare_non_inverting;
	uint8_t output_compare_inverting;
};

#define GPIO(PORT, PIN) ((struct gpio) { \
	.index = GPIO_INDEX_##PORT, \
	.input = GPIO_INPUT_##PORT, \
	.direction = GPIO_DIRECTION_##PORT, \
	.data = GPIO_DATA_##PORT, \
	.pin = PIN \
})

#define GPIO_TIMER(PORT, PIN) ((struct gpio_timer) { \
	.timer_control = GPIO_TIMER_CONTROL_##PORTD_##PIN, \
	.output_compare_low = GPIO_TIMER_OC_LOW_##PORTD_##PIN, \
	.output_compare_high = GPIO_TIMER_OC_HIGH_##PORTD_##PIN, \
	.output_compare_off = GPIO_TIMER_OC_OFF_##PORTD_##PIN, \
	.output_compare_non_inverting = GPIO_TIMER_OC_NINV_##PORTD_##PIN, \
	.output_compare_inverting = GPIO_TIMER_OC_INV_##PORTD_##PIN \
})

#define GPIO_INDEX_PORTB 0
#define GPIO_INDEX_PORTC 1
#define GPIO_INDEX_PORTD 2

#define GPIO_INPUT_PORTB &PINB
#define GPIO_INPUT_PORTC &PINC
#define GPIO_INPUT_PORTD &PIND

#define GPIO_DIRECTION_PORTB &DDRB
#define GPIO_DIRECTION_PORTC &DDRC
#define GPIO_DIRECTION_PORTD &DDRD

#define GPIO_DATA_PORTB &PORTB
#define GPIO_DATA_PORTC &PORTC
#define GPIO_DATA_PORTD &PORTD

#define GPIO_TIMER_CONTROL_PORTD_PIN6 &TCCR0A
#define GPIO_TIMER_OC_LOW_PORTD_PIN6 &OCR0A
#define GPIO_TIMER_OC_HIGH_PORTD_PIN6 NULL
#define GPIO_TIMER_OC_OFF_PORTD_PIN6 (uint8_t) ~(1 << COM0A0 | 1 << COM0A1)
#define GPIO_TIMER_OC_NINV_PORTD_PIN6 (1 << COM0A1)
#define GPIO_TIMER_OC_INV_PORTD_PIN6 (1 << COM0A0 | 1 << COM0A1)

#endif
