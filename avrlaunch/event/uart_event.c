#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/event/event.h"
#include "avrlaunch/event/uart_event.h"
#include "avrlaunch/hal/hal_uart.h"

typedef struct uart_buffer {
    uint8_t size;
    uint8_t start;
    uint8_t count;
    char chars[8];
} uart_buffer;

static volatile uart_buffer buffer = {
	.size = UART_BUFFER_SIZE,
	.start = 0,
	.count = 0
};

static event current_event;

static event* uart_poll_handler(event_descriptor* descriptor);
static char pop_uart_buffer(void);

ISR(USART_RX_vect) {
	char c = UDR0;
	if (c == '\r') {
		c = '\n';
	}
	int end = (buffer.start + buffer.count) % buffer.size;
	buffer.chars[end] = c;
	if (buffer.count == buffer.size) {
		buffer.start = (buffer.start + 1) % buffer.size;
	} else {
		buffer.count++;
	}
}

void uart_event_add_listener(event_handler handler) {
  event_register_source((event_descriptor) { EVENT_CATEGORY_UART, 0 }, 50, uart_poll_handler);
  event_add_listener((event_descriptor) { EVENT_CATEGORY_UART, 0 }, handler);
}

void uart_event_remove_listeners() {
  event_remove_listeners((event_descriptor) { EVENT_CATEGORY_UART, 0 });
}

static event* uart_poll_handler(event_descriptor* descriptor) {
	char c = pop_uart_buffer();
	if (c != 0) {
    current_event.descriptor = *descriptor;
    current_event.value = c;
		return &current_event;
	}
	return NULL;
}

static char pop_uart_buffer() {
	if (buffer.count > 0) {
		char c = buffer.chars[buffer.start];
	    buffer.start = (buffer.start + 1) % buffer.size;
	    buffer.count--;
		return c;
	} else {
		return 0;
	}
}
