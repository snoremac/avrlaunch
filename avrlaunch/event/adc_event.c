
#include <stdlib.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/event/event.h"
#include "avrlaunch/event/adc_event.h"
#include "avrlaunch/hal/hal_adc.h"

static volatile uint16_t last_readings[6];
static volatile uint16_t eventhresholds[6];

static event current_event;

static adc_reading read(uint8_t channel);
static event* adc_poll_handler(event_descriptor* descriptor);

void adc_event_add_listener(uint8_t channel, uint16_t eventhreshold, event_handler handler) {
	eventhresholds[channel] = eventhreshold;
	adc_read(channel);

	// ADC reads zero if you poll it too fast.  Not sure if 100ms is optimal though.
  event_register_source((event_descriptor) { EVENT_CATEGORY_ADC, channel }, 100,
			(event_poll_handler) adc_poll_handler);
  event_add_listener((event_descriptor) { EVENT_CATEGORY_ADC, channel }, handler);
}

void adc_event_remove_listeners(uint8_t channel) {
  event_remove_listeners((event_descriptor) { EVENT_CATEGORY_ADC, channel });
}

static adc_reading read(uint8_t channel) {
	uint16_t current_state = adc_read(channel);
	adc_reading reading = {
		current_state,
		last_readings[channel]
	};

	last_readings[channel] = current_state;
	return reading;
}

static event* adc_poll_handler(event_descriptor* descriptor) {
	uint8_t channel = descriptor->address;
	adc_reading reading = read(channel);

	if (reading.current_state == reading.previous_state) return NULL;
	if (abs(abs(reading.current_state) - abs(reading.previous_state)) < eventhresholds[channel]) return NULL;

	current_event.descriptor = *descriptor;
	current_event.value = reading.current_state;
	return &current_event;
}
