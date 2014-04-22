
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/clock.h"
#include "avrlaunch/log.h"
#include "avrlaunch/shell.h"
#include "avrlaunch/pgmspace/pgm_strings.h"
#include "avrlaunch/hal/hal_clock.h"

#define CLOCK_PRESCALE 64

static const double TICK_MILLIS = (1.0 / (F_CPU / CLOCK_PRESCALE)) * 1000;
static const double OVERFLOW_MILLIS = (1.0 / (F_CPU / CLOCK_PRESCALE)) * 1000 * 256;
static const double MILLI_OVERFLOWS = (F_CPU / 1000.0) / 256 / CLOCK_PRESCALE;

static uint32_t total_overflows;

static shell_result clock_shell_handler(shell_command* command);

void clock_init() {
	clock_timer_init();

  PGM_STR(CLOCK_SHELL_UPTIME, shell_uptime)
  shell_register_handler(shell_uptime, clock_shell_handler);
}

void clock_add_time(uint16_t millis) {
	total_overflows += (uint16_t) round(millis * MILLI_OVERFLOWS);
}

time clock_get_time() {
	total_overflows += clock_timer_consume_overflows();
	return total_overflows * OVERFLOW_MILLIS;
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

