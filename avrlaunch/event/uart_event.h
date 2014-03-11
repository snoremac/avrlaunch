#ifndef AVRLAUNCH_EVENT_UART_EVENT_H
#define AVRLAUNCH_EVENT_UART_EVENT_H

#include <inttypes.h>

#include "avrlaunch/event/event.h"

#define EVENT_TYPE_UART 0x02
#define EVENT_DESCRIPTOR_UART_RX 0x01
#define UART_BUFFER_SIZE 8

typedef struct uart_char_event {
	event super;
	char uart_char;
} uart_char_event;

void uart_event_add_listener(event_handler handler);

void uart_event_remove_listeners(void);

#endif
