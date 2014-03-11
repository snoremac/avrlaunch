#ifndef AVRLAUNCH_HAL_ADC_H
#define AVRLAUNCH_HAL_ADC_H

#include <inttypes.h>
#include <stdint.h>

#include "hal.h"
#include HAL_MODULE(adc)

void adc_init(void);

void adc_enable_dummy(uint8_t channel);

uint16_t adc_read(uint8_t channel);

#endif
