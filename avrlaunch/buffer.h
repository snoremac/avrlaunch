#ifndef AVRLAUNCH_BUFFER_H
#define AVRLAUNCH_BUFFER_H

#include <inttypes.h>
#include <stdio.h>

typedef enum buffer_mode {
	BUFFER_FIXED, BUFFER_CIRCULAR
} buffer_mode;

struct buffer {
  uint16_t element_size;
  uint16_t max_elements;
  uint16_t start_element;
  uint16_t element_count;
  uint8_t* data;
  buffer_mode mode;
};

void buffer_init(struct buffer* buffer, void* data, const uint16_t max_elements, const uint16_t element_size, buffer_mode mode);

void buffer_push(struct buffer* buffer, const void* value);

uint8_t buffer_uint8_at(struct buffer* buffer, uint16_t pos);

int8_t buffer_int8_at(struct buffer* buffer, uint16_t pos);

uint16_t buffer_uint16_at(struct buffer* buffer, uint16_t pos);

int16_t buffer_int16_at(struct buffer* buffer, uint16_t pos);

void buffer_dump_uint8(struct buffer* buffer, FILE* stream);

void buffer_dump_uint16(struct buffer* buffer, FILE* stream);

#endif
