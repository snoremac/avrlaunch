
#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/buffer/buffer.h"
#include "avrlaunch/event/event.h"
#include "avrlaunch/event/buffer_event.h"

static struct buffer* last_event_buffer;
static buffer_event_type last_buffer_event_type;
static buffer_event current_buffer_event;

static event* buffer_event_poll_handler(event_type type, event_descriptor descriptor);

void buffer_event_add_listener(struct buffer* buffer, buffer_event_handler handler) {
  event_register_source(EVENT_TYPE_BUFFER, (event_descriptor) buffer, 0, buffer_event_poll_handler);
  event_add_listener(EVENT_TYPE_BUFFER, (event_descriptor) buffer, (event_handler) handler);
}

void buffer_event_remove_listeners(struct buffer* buffer) {
  event_remove_listeners(EVENT_TYPE_BUFFER, (event_descriptor) buffer);
  event_deregister_source(EVENT_TYPE_BUFFER, (event_descriptor) buffer);
}

static event* buffer_event_poll_handler(event_type type, event_descriptor descriptor) {
  struct buffer* buffer = (struct buffer*) descriptor;
  if (buffer->element_count == buffer->max_elements) {
    current_buffer_event.super.type = EVENT_TYPE_BUFFER;
    current_buffer_event.super.descriptor = descriptor;
    current_buffer_event.buffer_event_type = BUFFER_FULL;
    current_buffer_event.buffer = (struct buffer*) descriptor;
    return (event*) &current_buffer_event;
  }
  return NULL;
}
