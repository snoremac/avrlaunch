
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/clock.h"
#include "avrlaunch/shell.h"
#include "avrlaunch/pgmspace/pgm_strings.h"

static const double OVERFLOW_MILLIS = (1.0 / (F_CPU / 64)) * 256 * 1000;

static uint8_t overflow_millis_rounded;
static uint16_t overflow_micros;

static volatile time time_millis;
static volatile time time_micros;

static shell_result clock_shell_handler(shell_command* command);

ISR(TIMER0_OVF_vect) {
	// TODO: Count overflows only
	time_millis += overflow_millis_rounded;
	time_micros += overflow_micros;
	if (time_micros > 1000) {
		time_millis++;
		time_micros %= 1000;
	}
}

void clock_init(void) {
	// Timer 0, normal mode
	clear_bit(&TCCR0A, WGM00);
	clear_bit(&TCCR0A, WGM01);
	clear_bit(&TCCR0A, WGM02);

	// Timer 0, prescale 64
	set_bit(&TCCR0B, CS00);
	set_bit(&TCCR0B, CS01);
	clear_bit(&TCCR0B, CS02);

	// Timer 0, disconnect OCs
	clear_bit(&TCCR0A, COM0A0);
	clear_bit(&TCCR0A, COM0A1);
	clear_bit(&TCCR0A, COM0B0);
	clear_bit(&TCCR0A, COM0B1);

  // Timer 0, enable overflow interrupt
  set_bit(&TIMSK0, TOIE0);

	overflow_millis_rounded = round(OVERFLOW_MILLIS);
	overflow_micros = (uint16_t) round((OVERFLOW_MILLIS - overflow_millis_rounded) * 1000);
  
  PGM_STR(CLOCK_SHELL_UPTIME, shell_uptime)
  shell_register_handler(shell_uptime, clock_shell_handler);
}

void clock_set_time(time millis) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    time_millis = millis;
	  time_micros = 0;
  }
}

time clock_get_time() {
	time millis;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		millis = time_millis;
	}
	return millis;
}

void clock_format_time(time millis, char* output, uint8_t output_length) {
	if (millis < 60000) {
  	PGM_STR(CLOCK_SHELL_FORMAT_SECONDS, format);		
		snprintf(output, output_length, format, millis / 1000);						
	} else if (millis < 3600000) {
  	PGM_STR(CLOCK_SHELL_FORMAT_MINUTES, format);		
		snprintf(output, output_length, format, millis / 60000, (millis % 60000) / 1000);								
	}
}

static shell_result clock_shell_handler(shell_command* command) {
  PGM_STR(CLOCK_SHELL_UPTIME, shell_uptime)

	if (strcmp(command->command, shell_uptime) == 0) {
		char formatted_time[20];																
		clock_format_time(clock_get_time(), formatted_time, 20);
  	PGM_STR(CLOCK_SHELL_FORMAT_UPTIME, format);		
		shell_printf(format, formatted_time);		
		return SHELL_RESULT_SUCCESS;
	}
	return SHELL_RESULT_FAIL;
}

