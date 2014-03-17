
#include <inttypes.h>
#include <stdio.h>
#include <avr/eeprom.h>

#include "avrlaunch/buffer.h"

void buffer_init(struct buffer* buffer, uint8_t* slots, const uint16_t size, buffer_mode mode) {
    buffer->size = size;
    buffer->start = 0;
    buffer->count = 0;
    buffer->slots = slots;
    buffer->mode = mode;
}
 
void buffer_push(struct buffer* buffer, const uint8_t value) {
  uint16_t end = (buffer->start + buffer->count) % buffer->size;
  
  if (buffer->count < buffer->size || buffer->mode == BUFFER_CIRCULAR) {
    buffer->slots[end] = value;
  }

  if (buffer->count == buffer->size) {
    if (buffer->mode == BUFFER_CIRCULAR) {
      buffer->start = (buffer->start + 1) % buffer->size;            
    }
  } else {
    buffer->count++;
  }
}

uint8_t buffer_at(struct buffer* buffer, uint16_t pos) {
  uint16_t real_pos = (buffer->start + pos) % buffer->size;
  return buffer->slots[real_pos];
}

void buffer_load(struct buffer* buffer, uint16_t pos) {
  eeprom_busy_wait();
  uint16_t length = eeprom_read_word((uint16_t*) pos);
  pos += 2;
  
  for (uint16_t i = 0; i < length; i++) {
    eeprom_busy_wait();  
    buffer_push(buffer, eeprom_read_byte((uint8_t*) pos + i));
  }
}

void buffer_save(struct buffer* buffer, uint16_t pos) {
  eeprom_busy_wait();
  eeprom_update_word((uint16_t*) pos, buffer->count);
  pos += 2;
  
  for (uint16_t i = 0; i < buffer->count; i++) {
    eeprom_busy_wait();  
    eeprom_update_byte((uint8_t*) pos + i, buffer_at(buffer, i));
  }
}

void buffer_dump(struct buffer* buffer, FILE* stream) {
  for (uint16_t i = 0; i < buffer->count; i++) {
    fprintf(stream, "%u", buffer_at(buffer, i));
    if (i + 1 < buffer->count) {
      fputc(',', stream);      
    }
    if ((i + 1) % 25 == 0) {
      fputc('\n', stream);
    }    
  }
  fputc('\n', stream);
  fputc('\n', stream);
}

