#ifndef AVRLAUNCH_BUFFER_H
#define AVRLAUNCH_BUFFER_H

#include <inttypes.h>
#include <stdio.h>

typedef enum buffer_mode {
	BUFFER_FIXED, BUFFER_CIRCULAR
} buffer_mode;

struct buffer {
  uint16_t size;
  uint16_t start;
  uint16_t count;
  uint8_t* slots;
  buffer_mode mode;
};

void buffer_init(struct buffer* buffer, uint8_t* slots, const uint16_t size, buffer_mode mode);

void buffer_push(struct buffer* buffer, const uint8_t value);

uint8_t buffer_at(struct buffer* buffer, uint16_t pos);

void buffer_load(struct buffer* buffer, uint16_t pos);

void buffer_save(struct buffer* buffer, uint16_t pos);

void buffer_dump(struct buffer* buffer, FILE* stream);

#endif
