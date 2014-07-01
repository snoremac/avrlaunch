
#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/log.h"
#include "avrlaunch/hal/atmega32u4/hal_clock.h"

static uint8_t overflows;

void clock_timer_init() {
	// Timer 0
	// Normal mode
	clear_bit(&TCCR0A, WGM00);
	clear_bit(&TCCR0A, WGM01);
	clear_bit(&TCCR0B, WGM02);

	// Prescale 64
	set_bit(&TCCR0B, CS00);
	set_bit(&TCCR0B, CS01);
	clear_bit(&TCCR0B, CS02);

	// Disconnect OCs
	clear_bit(&TCCR0A, COM0A0);
	clear_bit(&TCCR0A, COM0A1);
	clear_bit(&TCCR0A, COM0B0);
	clear_bit(&TCCR0A, COM0B1);

	// Enable overflow interrupt
	set_bit(&TIMSK0, TOIE0);
}

uint8_t clock_timer_consume_overflows() {
	uint8_t result = 0;
		result = overflows;
		overflows = 0;		
	return result;
}

void clock_timer_add_overflows(uint8_t overflows_added) {
	overflows += overflows_added;
}

ISR(TIMER0_OVF_vect) {
	overflows++;
}
