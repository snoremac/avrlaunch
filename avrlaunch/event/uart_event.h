#ifndef AVRLAUNCH_EVENT_UART_EVENT_H
#define AVRLAUNCH_EVENT_UART_EVENT_H

#include <inttypes.h>

#include "avrlaunch/event/event.h"

#define EVENT_CATEGORY_UART 2
#define UART_BUFFER_SIZE 8

void uart_event_add_listener(event_handler handler);

void uart_event_remove_listeners(void);

#endif
