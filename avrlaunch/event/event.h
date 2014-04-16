#ifndef AVRLAUNCH_EVENT_EVENT_H
#define AVRLAUNCH_EVENT_EVENT_H

#include <inttypes.h>
#include <stdbool.h>

#include "avrlaunch/avrlaunch.h"

#ifndef EVENT_MAX_SOURCES
#define EVENT_MAX_SOURCES 0
#endif

#ifndef EVENT_MAX_LISTENERS
#define EVENT_MAX_LISTENERS 0
#endif

#define EVENT_POLL_INTERVAL_ASAP 0
#define EVENT_POLL_INTERVAL_MAX UINT16_MAX

typedef uint8_t event_category;
typedef uint16_t event_address;

typedef struct event_descriptor {
	event_category category;
	event_address address;
} event_descriptor;

typedef struct event {
	event_descriptor descriptor;
} event;

typedef event* (*event_poll_handler)(event_descriptor* descriptor);
typedef bool (*event_handler)(event* event);

void event_init(void);

void event_tick(void);

void event_fire_event(event* event);

void event_register_source(event_descriptor descriptor,
		uint16_t poll_interval, event_poll_handler poll_handler);

void event_deregister_source(event_descriptor descriptor);

void event_add_listener(event_descriptor descriptor, event_handler handler);

void event_remove_listeners(event_descriptor descriptor);

void event_clear_sources(void);

void event_clear_listeners(void);

#endif
