#ifndef AVRLAUNCH_EVENT_EVENT_INTERNAL_H
#define AVRLAUNCH_EVENT_EVENT_INTERNAL_H

#include <inttypes.h>
#include <stdbool.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/event/event.h"

typedef struct event_callable {
  bool active;
	event_descriptor descriptor;
} event_callable;

typedef struct event_source {
	event_callable super;
	uint16_t poll_interval;
	event_poll_handler poll_handler;
  time next_scheduled_poll;
} event_source;

typedef struct event_listener {
	event_callable super;
	event_handler handler;
} event_listener;

uint8_t event_source_count(void);

uint8_t event_listener_count(void);

event_source* first_event_source(void);

event_listener* first_event_listener(void);


#endif
