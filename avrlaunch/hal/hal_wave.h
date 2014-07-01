#ifndef AVRLAUNCH_HAL_WAVE_H
#define AVRLAUNCH_HAL_WAVE_H

#include <inttypes.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/hal/hal.h"
#include HAL_MODULE(wave)

typedef enum wave_edge {
  WAVE_EDGE_RISING, WAVE_EDGE_FALLING, WAVE_EDGE_BOTH
} wave_edge;

typedef void (*wave_input_capture_callback)(uint16_t ticks_since_previous);
typedef uint8_t (*wave_duty_cycle_callback)(void);

void wave_in_timer_init(void);

void wave_in_timer_on(wave_input_capture_callback callback, wave_edge capture_edge);

void wave_in_timer_off(void);

void wave_in_timer_reset(void);

uint16_t wave_in_current_tick(void);

void wave_out_timer_init(void);

void wave_out_timer_on(wave_duty_cycle_callback callback);

void wave_out_timer_off(void);

#endif
