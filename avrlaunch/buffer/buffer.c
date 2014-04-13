
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <avr/eeprom.h>
#include <util/atomic.h>

#include "avrlaunch/buffer/buffer.h"
#include "avrlaunch/log.h"

struct buffer buffer_init(volatile void* data, const uint16_t max_elements, const uint16_t element_size) {
    struct buffer buffer = (struct buffer) {
      .data = data,
      .max_elements = max_elements,
      .element_size = element_size,
      .start_element = 0,
      .element_count = 0
    };
    memset((void*) data, 0, max_elements * element_size);
    return buffer;
}

void buffer_push_overflow(struct buffer* buffer, volatile void* value) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    uint16_t next_element = (buffer->start_element + buffer->element_count) % buffer->max_elements;
    memcpy((void*) buffer->data + (next_element * buffer->element_size), (void*) value, buffer->element_size);

    if (buffer->element_count == buffer->max_elements) {
      buffer->start_element = (buffer->start_element + 1) % buffer->max_elements;
    } else {
      buffer->element_count++;
    }
  }
}

void buffer_push_no_overflow(struct buffer* buffer, volatile void* value) {
  if (buffer->element_count < buffer->max_elements) {
	  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      uint16_t next_element = (buffer->start_element + buffer->element_count) % buffer->max_elements;
      memcpy((void*) buffer->data + (next_element * buffer->element_size), (void*) value, buffer->element_size);
      buffer->element_count++;
    }
  } else {
    set_system_warning();
  }
}

volatile void* buffer_shift(struct buffer* buffer) {
  volatile void* element = NULL;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    element = buffer->data + (buffer->start_element * buffer->element_size);
    buffer->start_element++;
    if (buffer->start_element == buffer->max_elements) {
      buffer->start_element = 0;
    }
    buffer->element_count--;
  }
  return element;
}

volatile void* buffer_at(struct buffer* buffer, uint16_t pos) {
  uint16_t real_pos = (buffer->start_element + pos) % buffer->max_elements;
  return (buffer->data + (real_pos * buffer->element_size));
}

void buffer_dump(struct buffer* buffer, element_print_handler handler, FILE* stream) {
  for (uint16_t i = 0; i < buffer->element_count; i++) {
    handler(buffer, i, stream);
    if (i + 1 < buffer->element_count) {
      fputc(',', stream);
    }
    if ((i + 1) % 25 == 0) {
      fputc('\n', stream);
    }
  }
  fputc('\n', stream);
  fputc('\n', stream);
}
