#ifndef AVRLAUNCH_EVENT_BUFFER_EVENT_H
#define AVRLAUNCH_EVENT_BUFFER_EVENT_H

#include <stdbool.h>

#include "avrlaunch/event/event.h"

#define EVENT_TYPE_BUFFER 4

typedef enum buffer_event_type {
	BUFFER_FULL, BUFFER_NOT_FULL
} buffer_event_type;

typedef struct buffer_event {
	event super;
	buffer_event_type event_type;
	struct buffer* buffer;
} buffer_event;

typedef bool (*buffer_event_handler)(buffer_event* event);

void buffer_event_add_listener(struct buffer* buffer, buffer_event_handler handler);

void buffer_event_remove_listeners(struct buffer* buffer);

#endif
