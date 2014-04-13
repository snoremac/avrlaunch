#ifndef AVRLAUNCH_BUFFER_INT_H
#define AVRLAUNCH_BUFFER_INT_H

#include <inttypes.h>
#include <stdio.h>

#include "avrlaunch/buffer/buffer.h"

uint8_t buffer_shift_uint8(struct buffer* buffer);

int8_t buffer_shift_int8(struct buffer* buffer);

uint16_t buffer_shift_uint16(struct buffer* buffer);

int16_t buffer_shift_int16(struct buffer* buffer);

uint8_t buffer_uint8_at(struct buffer* buffer, uint16_t pos);

int8_t buffer_int8_at(struct buffer* buffer, uint16_t pos);

uint16_t buffer_uint16_at(struct buffer* buffer, uint16_t pos);

int16_t buffer_int16_at(struct buffer* buffer, uint16_t pos);

void buffer_dump_uint8(struct buffer* buffer, FILE* stream);

void buffer_dump_uint16(struct buffer* buffer, FILE* stream);

#endif
