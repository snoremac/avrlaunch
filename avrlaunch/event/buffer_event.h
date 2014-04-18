#ifndef AVRLAUNCH_EVENT_BUFFER_EVENT_H
#define AVRLAUNCH_EVENT_BUFFER_EVENT_H

#include <stdbool.h>

#include "avrlaunch/event/event.h"

#define EVENT_CATEGORY_BUFFER 4

#define BUFFER_NONE 0x00
#define BUFFER_FULL 0x01
#define BUFFER_NOT_FULL 0x02

#ifndef MAX_BUFFER_STATES
#define MAX_BUFFER_STATES 2
#endif

void buffer_event_init(void);

void buffer_event_add_listener(struct buffer* buffer, event_handler handler);

void buffer_event_remove_listeners(struct buffer* buffer);

#endif
