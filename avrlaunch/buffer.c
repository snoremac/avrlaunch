
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <avr/eeprom.h>

#include "avrlaunch/buffer.h"
#include "avrlaunch/log.h"

typedef void (*element_print_handler)(struct buffer* buffer, uint16_t pos, FILE* stream);

static void* buffer_at(struct buffer* buffer, uint16_t pos);
static void buffer_dump(struct buffer* buffer, element_print_handler handler, FILE* stream);
static void print_uint8(struct buffer* buffer, uint16_t pos, FILE* stream);
static void print_uint16(struct buffer* buffer, uint16_t pos, FILE* stream);

void buffer_init(struct buffer* buffer, void* data, const uint16_t max_elements, const uint16_t element_size, buffer_mode mode) {
    buffer->data = data;
    buffer->max_elements = max_elements;
    buffer->element_size = element_size;
    buffer->mode = mode;

    memset(data, 0, max_elements * element_size);
    buffer->start_element = 0;
    buffer->element_count = 0;
}
 
void buffer_push(struct buffer* buffer, const void* value) {
  uint16_t next_element = (buffer->start_element + buffer->element_count) % buffer->max_elements;
  if (buffer->element_count < buffer->max_elements || buffer->mode == BUFFER_CIRCULAR) {
    memcpy(buffer->data + (next_element * buffer->element_size), value, buffer->element_size);
  }

  if (buffer->element_count == buffer->max_elements) {
    if (buffer->mode == BUFFER_CIRCULAR) {
      buffer->start_element = (buffer->start_element + 1) % buffer->max_elements;
    }
  } else {
    buffer->element_count++;
  }
}

static void* buffer_at(struct buffer* buffer, uint16_t pos) {
  uint16_t real_pos = (buffer->start_element + pos) % buffer->max_elements;
  return (buffer->data + (real_pos * buffer->element_size));
}

uint8_t buffer_uint8_at(struct buffer* buffer, uint16_t pos) {
  return *((uint8_t*) buffer_at(buffer, pos));
}

int8_t buffer_int8_at(struct buffer* buffer, uint16_t pos) {
  return *((int8_t*) buffer_at(buffer, pos));
}

uint16_t buffer_uint16_at(struct buffer* buffer, uint16_t pos) {
  return *((uint16_t*) buffer_at(buffer, pos));
}

int16_t buffer_int16_at(struct buffer* buffer, uint16_t pos) {
  return *((int16_t*) buffer_at(buffer, pos));
}

void buffer_dump_uint8(struct buffer* buffer, FILE* stream) {
  buffer_dump(buffer, print_uint8, stream);
}

void buffer_dump_uint16(struct buffer* buffer, FILE* stream) {
  buffer_dump(buffer, print_uint16, stream);
}

static void print_uint8(struct buffer* buffer, uint16_t pos, FILE* stream) {
  fprintf(stream, "%u", buffer_uint8_at(buffer, pos));
}

static void print_uint16(struct buffer* buffer, uint16_t pos, FILE* stream) {
  fprintf(stream, "%u", buffer_uint16_at(buffer, pos));
}

static void buffer_dump(struct buffer* buffer, element_print_handler handler, FILE* stream) {
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

