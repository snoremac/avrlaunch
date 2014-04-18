
#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/log.h"
#include "avrlaunch/buffer/buffer.h"
#include "avrlaunch/event/event.h"
#include "avrlaunch/event/buffer_event.h"

struct buffer_state {
  struct buffer* buffer;
  uint8_t flags;
};

static struct buffer buffer_state_buffer;
static struct buffer_state buffer_state_buffer_data[MAX_BUFFER_STATES];

static event current_event;

static event* buffer_poll_handler(event_descriptor* descriptor);
static uint8_t buffer_flags(struct buffer* buffer);
static uint8_t last_buffer_flags(struct buffer* buffer);

void buffer_event_init() {
  buffer_state_buffer = buffer_init(buffer_state_buffer_data, MAX_BUFFER_STATES, sizeof(struct buffer_state));
}

void buffer_event_add_listener(struct buffer* buffer, event_handler handler) {
  event_register_source((event_descriptor) { EVENT_CATEGORY_BUFFER, (uint16_t) buffer }, 0, buffer_poll_handler);
  event_add_listener((event_descriptor) { EVENT_CATEGORY_BUFFER, (uint16_t) buffer }, (event_handler) handler);
}

void buffer_event_remove_listeners(struct buffer* buffer) {
  event_remove_listeners((event_descriptor) { EVENT_CATEGORY_BUFFER, (uint16_t) buffer });
}

static event* buffer_poll_handler(event_descriptor* descriptor) {
  struct buffer* buffer = (struct buffer*) descriptor->address;
  uint8_t current_flags = buffer_flags(buffer);
  uint8_t last_flags = last_buffer_flags(buffer);
  if (current_flags != last_flags) {
    buffer_push_overflow(&buffer_state_buffer, &((struct buffer_state) { buffer, current_flags }));
    if (last_flags != BUFFER_NONE) {
      current_event.descriptor = *descriptor;
      current_event.flags = current_flags;
      return &current_event;        
    }
  }
  return NULL;
}

static uint8_t buffer_flags(struct buffer* buffer) {
  return buffer->element_count == buffer->max_elements ? BUFFER_FULL : BUFFER_NOT_FULL;
}

static uint8_t last_buffer_flags(struct buffer* buffer) {
  for (uint16_t i = buffer_state_buffer.element_count; i-- > 0; ) {
    struct buffer_state* state = (struct buffer_state*) buffer_at(&buffer_state_buffer, i);
    if (state->buffer == buffer) {
      return state->flags;
    }
  }
  return BUFFER_NONE;
}
