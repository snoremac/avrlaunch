#ifndef AVRLAUNCH_HAL_CLOCK_H
#define AVRLAUNCH_HAL_CLOCK_H

#include <inttypes.h>
#include <stdint.h>

#include "hal.h"

#include HAL_MODULE(clock)

void clock_timer_init(void);

uint8_t clock_timer_consume_overflows(void);

void clock_timer_add_overflows(uint8_t overflows_added);

#endif
