
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <avr/eeprom.h>

#include "avrlaunch/buffer/buffer_int.h"
#include "avrlaunch/log.h"

static void print_uint8(struct buffer* buffer, uint16_t pos, FILE* stream);
static void print_uint16(struct buffer* buffer, uint16_t pos, FILE* stream);

uint8_t buffer_shift_uint8(struct buffer* buffer) {
  return *((uint8_t*) buffer_shift(buffer));
}

int8_t buffer_shift_int8(struct buffer* buffer) {
  return *((int8_t*) buffer_shift(buffer));
}

uint16_t buffer_shift_uint16(struct buffer* buffer) {
  return *((uint16_t*) buffer_shift(buffer));
}

int16_t buffer_shift_int16(struct buffer* buffer) {
  return *((int16_t*) buffer_shift(buffer));
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
