#ifndef AVRLAUNCH_BUFFER_H
#define AVRLAUNCH_BUFFER_H

#include <inttypes.h>
#include <stdio.h>

struct buffer {
  volatile void* data;
  const uint16_t max_elements;
  const uint16_t element_size;
  volatile uint16_t start_element;
  volatile uint16_t element_count;
};

typedef void (*element_print_handler)(struct buffer* buffer, uint16_t pos, FILE* stream);

struct buffer buffer_init(volatile void* data, const uint16_t max_elements, const uint16_t element_size);

void buffer_push_overflow(struct buffer* buffer, volatile void* value);

void buffer_push_no_overflow(struct buffer* buffer, volatile void* value);

volatile void* buffer_shift(struct buffer* buffer);

volatile void* buffer_at(struct buffer* buffer, uint16_t pos);

void buffer_dump(struct buffer* buffer, element_print_handler handler, FILE* stream);

#endif
