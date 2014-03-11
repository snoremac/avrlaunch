#ifndef AVRLAUNCH_SHELL_INTERNAL_H
#define AVRLAUNCH_SHELL_INTERNAL_H

#include <inttypes.h>
#include <stdbool.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/shell.h"

void shell_handler_init();

shell_result shell_invoke(const char* command_line);

uint8_t shell_handler_count(void);

shell_handler* shell_first_handler(void);

bool is_handler_registered(const char* command);

shell_handler* lookup_handler(const char* command);

#endif
