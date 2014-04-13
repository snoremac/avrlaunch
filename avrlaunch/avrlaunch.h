#ifndef AVRLAUNCH_AVRLAUNCH_H
#define AVRLAUNCH_AVRLAUNCH_H

#include <stdint.h>
#include <stdbool.h>

#ifdef AVRLAUNCH_OVERRIDE_DEFS
#include "avrlaunch/defs.h"
#endif

#define ARRAY_SIZE(x) (sizeof((x)) / sizeof((x)[0]))

#define not(expr) !(expr)

#define null(expr) expr == NULL
#define not_null(expr) expr != NULL

#define success(result_func) result_func == RESULT_SUCCESS
#define not_success(result_func) result_func != RESULT_SUCCESS

typedef enum result {
	RESULT_SUCCESS, RESULT_FAIL, RESULT_FAIL_BAD_ARG, RESULT_FAIL_CAPACITY,
	RESULT_FAIL_MATCH, RESULT_FAIL_DUPLICATE, RESULT_UNKNOWN
} result;

typedef enum logic_level {
	LOGIC_UNKNOWN, LOGIC_LOW, LOGIC_HIGH
} logic_level;

typedef uint8_t event_type;
typedef uint16_t event_descriptor;
typedef uint32_t time;

typedef void (*error_callback)(const char* message);

logic_level read_bit(volatile uint8_t *byte, uint8_t bit);

void set_bit(volatile uint8_t *byte, uint8_t bit);

void toggle_bit(volatile uint8_t *byte, uint8_t bit);

void clear_bit(volatile uint8_t *byte, uint8_t bit);

uint8_t high_byte(uint16_t word);

uint8_t low_byte(uint16_t word);

uint16_t high_word(uint32_t long_word);

uint16_t low_word(uint32_t long_word);

uint8_t msb(uint16_t byte);

void delay(uint16_t millis);

bool string_eq(const char* string_1, const char* string_2);

bool string_eq_i(const char* string_1, const char* string_2);

void system_reset(void);

#endif
