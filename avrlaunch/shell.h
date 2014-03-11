#ifndef AVRLAUNCH_SHELL_H
#define AVRLAUNCH_SHELL_H

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "avrlaunch/avrlaunch.h"

#ifndef SHELL_CMD_MAX_LENGTH
#define SHELL_CMD_MAX_LENGTH 16
#endif

#ifndef SHELL_MAX_HANDLERS
#define SHELL_MAX_HANDLERS 4
#endif

#define SHELL_PROMPT_MAX_LENGTH 8
#define SHELL_TOKEN_MAX_LENGTH 8

typedef enum shell_result {
	SHELL_RESULT_SUCCESS, SHELL_RESULT_ENTER_MODAL, SHELL_RESULT_EXIT_MODAL, SHELL_RESULT_FAIL
} shell_result;

typedef struct shell_command {
	char* command;
	char** args;
	uint8_t args_count;
} shell_command;

typedef struct shell_handler {
  bool active;
	char command[SHELL_TOKEN_MAX_LENGTH];
	shell_result (*handler)(shell_command* command);
} shell_handler;

typedef struct shell_state {
	shell_handler* active_handler;
	bool modal;
} shell_state;

void shell_init();

void shell_printf(const char* format, ...);

result shell_register_handler(const char* command, shell_result (*handler)(shell_command* command));

void shell_clear_handlers(void);

FILE* shell_get_stream(void);

#endif
