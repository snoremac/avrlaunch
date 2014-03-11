
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>
#include <avr/wdt.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/clock.h"
#include "avrlaunch/shell.h"
#include "avrlaunch/scheduler.h"
#include "avrlaunch/event/event.h"
#include "avrlaunch/hal/hal_adc.h"
#include "avrlaunch/hal/hal_gpio.h"
#include "avrlaunch/hal/hal_uart.h"

#ifdef SIM
#include "avr/avr_mcu_section.h"
AVR_MCU(F_CPU, "atmega328p");
#endif

int main() {
  wdt_disable();

  adc_init();
	clock_init();
	scheduler_init();
	event_init();
	shell_init();

  struct task_config setup_config = { "setup", TASK_ONCE, TASK_ASAP };
	scheduler_add_task(&setup_config, setup_task, NULL);

	while (1) {
		scheduler_tick();
		event_tick();
	}
}

logic_level read_bit(volatile uint8_t* byte, uint8_t bit) {
	return (_BV(bit) & *byte) == _BV(bit) ? LOGIC_HIGH : LOGIC_LOW;	
}

void set_bit(volatile uint8_t *byte, uint8_t bit) {
	*byte |= _BV(bit);
}

void toggle_bit(volatile uint8_t *byte, uint8_t bit) {
	*byte ^= _BV(bit);
}

void clear_bit(volatile uint8_t *byte, uint8_t bit) {
	*byte &= ~_BV(bit);
}

uint8_t high_byte(uint16_t word) {
	return word >> 0x08;
}

uint8_t low_byte(uint16_t word) {
	return word & 0xff;
}

uint16_t high_word(uint32_t long_word) {
	return long_word >> 0x10;
}

uint16_t low_word(uint32_t long_word) {
	return long_word & 0xffff;
}

uint8_t msb(uint16_t byte) {
	uint8_t msb = 0;
	while (byte >>= 1) {
		msb++;
	}
	return msb;
}

void delay(uint16_t millis) {
	_delay_ms(millis);
}

bool string_eq(const char* string_1, const char* string_2) {
	return strcmp(string_1, string_2) == 0;
}

bool string_eq_i(const char* string_1, const char* string_2) {
	return strcasecmp(string_1, string_2) == 0;
}

void system_reset(void) {
  wdt_enable(WDTO_60MS);
}
