#ifndef AVR_KIT_EVENT_EVENT_H
#define AVR_KIT_EVENT_EVENT_H

#include <inttypes.h>
#include <stdbool.h>

#include "avrlaunch/avrlaunch.h"

#ifndef EVENT_MAX_SOURCES
#define EVENT_MAX_SOURCES 0
#endif

#ifndef EVENT_MAX_LISTENERS
#define EVENT_MAX_LISTENERS 0
#endif

#define EVENT_ASAP 0

typedef struct event {
	event_type type;
	event_descriptor descriptor;
} event;

typedef event* (*event_poll_handler)(event_type type, event_descriptor descriptor);
typedef bool (*event_handler)(event* event);

void event_init(void);

void event_tick(void);

void event_fire_event(event* event);

result event_register_source(event_type type, event_descriptor descriptor,
    time poll_interval, event_poll_handler poll_handler);

void event_deregister_source(event_type type, event_descriptor descriptor);

result event_add_listener(event_type type, event_descriptor descriptor, event_handler handler);

void event_remove_listeners(event_type type, event_descriptor descriptor);

void event_clear_sources(void);

void event_clear_listeners(void);

#endif
