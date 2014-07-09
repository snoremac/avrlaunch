#ifndef AVRLAUNCH_HAL_UART_H
#define AVRLAUNCH_HAL_UART_H

#include <inttypes.h>
#include <stdio.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/hal/hal.h"
#include HAL_MODULE(uart)

void uart_enable(uint32_t baud);

void uart_disable(void);

void uart_putc(char c);

int uart_fputc(FILE* stream, char c);

struct buffer* uart_get_buffer(void);

#endif
