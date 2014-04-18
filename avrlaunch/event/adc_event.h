#ifndef AVRLAUNCH_EVENT_ADC_EVENT_H
#define AVRLAUNCH_EVENT_ADC_EVENT_H

#include <inttypes.h>
#include <stdint.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/event/event.h"

#define EVENT_CATEGORY_ADC 3

typedef struct adc_reading {
	uint16_t current_state;
	uint16_t previous_state;
} adc_reading;

void adc_event_add_listener(uint8_t channel, uint16_t eventhreshold, event_handler handler);

void adc_event_remove_listeners(uint8_t channel);

#endif
