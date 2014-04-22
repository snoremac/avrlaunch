#ifndef AVRLAUNCH_CLOCK_H
#define AVRLAUNCH_CLOCK_H

#include <inttypes.h>

#include "avrlaunch/avrlaunch.h"

#define CLOCK_TIMER 0
#define CLOCK_ISR_VECT TIMER_0_OVERFLOW_VECT

void clock_init(void);

time clock_get_time(void);

void clock_add_time(uint16_t millis);

void clock_format_time(time millis, char* output, uint8_t output_length);

#endif
