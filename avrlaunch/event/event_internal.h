#ifndef AVR_KIT_EVENT_EVENT_INTERNAL_H
#define AVR_KIT_EVENT_EVENT_INTERNAL_H

#include <inttypes.h>
#include <stdbool.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/event/event.h"

typedef struct event_callable {
  bool active;
	event_type type;
	event_descriptor descriptor;
} event_callable;

typedef struct event_source {
	event_callable super;
	time poll_interval;
	time next_scheduled_poll;
	event_poll_handler poll_handler;
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
