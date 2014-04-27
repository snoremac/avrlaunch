#ifndef AVRLAUNCH_HAL_WAVE_H
#define AVRLAUNCH_HAL_WAVE_H

#include <inttypes.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/hal/hal.h"
#include HAL_MODULE(wave)

typedef uint8_t (*amplitude_callback)(void);

void wave_timer_init(void);

void wave_timer_on(amplitude_callback callback);

void wave_timer_off(void);

#endif
