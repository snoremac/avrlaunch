#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/shell.h"
#include "avrlaunch/shell_internal.h"
#include "avrlaunch/buffer/buffer.h"
#include "avrlaunch/buffer/buffer_int.h"
#include "avrlaunch/pgmspace/pgm_strings.h"
#include "avrlaunch/event/buffer_event.h"
#include "avrlaunch/hal/hal_uart.h"

static int uart_sgetc(FILE *stream);
static int uart_sputc(char c, FILE *stream);

static bool on_uart_buffer_event(event* e);
static void on_char(char c);
static void on_char_return();
static void on_char_backspace();

static void print_prompt();

static uint8_t uint8_tokens(const char* cmd_copy);
static void tokenise(char* cmd_copy, char** tokens, uint8_t tokens_length);
static shell_command new_shell_command(char** tokens, uint8_t token_count);
static void handle_failure(shell_result result, const char* command);
  	
static shell_result delegating_handler(shell_command* command);

FILE uart_stream = FDEV_SETUP_STREAM(uart_sputc, uart_sgetc, _FDEV_SETUP_RW);

static shell_handler default_handler = {
  .command = "",
	.handler = delegating_handler
};

static shell_state current_shell_state = {
  .active_handler = &default_handler
};

static char command_buffer[SHELL_CMD_MAX_LENGTH];
static uint8_t curr_index = 0;

void shell_init() {
  shell_handler_init();

	memset(command_buffer, '\0', SHELL_CMD_MAX_LENGTH);

  uart_enable(UART_BAUD);
  buffer_event_add_listener(get_uart_buffer(), on_uart_buffer_event);
  
	print_prompt();
}

void shell_printf(const char* format, ...) {
	va_list args;
	va_start(args, format);
	vfprintf(&uart_stream, format, args);
	va_end(args);
}

FILE* shell_get_stream(void) {
  return &uart_stream;
}

static int uart_sgetc(FILE *stream) {
  return 0;
}

static int uart_sputc(char c, FILE *stream) {
  uart_putc(c);
  return 0;
}

static bool on_uart_buffer_event(event* event) {
  if (event-> flags & BUFFER_HOLDING) {
  	char c = buffer_shift_uint8((struct buffer*) event->descriptor.address);
  	if (c != 0) {
    	if (c == '\n') {
        on_char_return();
      } else if (c == '\b') {
        on_char_backspace();
  		} else {
        on_char(c);
  		}
  	}
    
  }
	return true;
}

static void on_char(char c) {
	uart_putc(c);
	if (curr_index < SHELL_CMD_MAX_LENGTH - 1) {
		command_buffer[curr_index] = c;
		command_buffer[curr_index + 1] = '\0';
	}
  curr_index++;  
}

static void on_char_return() {
	uart_putc('\n');
	if (curr_index < SHELL_CMD_MAX_LENGTH - 1) {
		shell_invoke(command_buffer);        
  } else {
    PGM_STR(SHELL_CMD_TOO_LONG, failed_msg)
    shell_printf(failed_msg, SHELL_CMD_MAX_LENGTH);
  }

  print_prompt();
	memset(command_buffer, '\0', SHELL_CMD_MAX_LENGTH);
	curr_index = 0;  
}

static void on_char_backspace() {
  if (curr_index > 0) {
		uart_putc('\b');
		uart_putc(' ');
		uart_putc('\b');
		command_buffer[curr_index - 1] = '\0';
    curr_index--;        
  }  
}

static void print_prompt() {
  shell_printf(current_shell_state.active_handler->command);    
  shell_printf(">");    
}

shell_result shell_invoke(const char* cmd_line) {
  uint8_t token_count = uint8_tokens(cmd_line);
  if (token_count == 0 || strlen(cmd_line) == 0) {
    return RESULT_SUCCESS;
  }
  
	char cmd_copy[strlen(cmd_line) + 1];
	strncpy(cmd_copy, cmd_line, strlen(cmd_line) + 1);
	char* tokens[token_count];
  tokenise(cmd_copy, tokens, token_count);
  
  shell_command command = new_shell_command(tokens, token_count);
	shell_result result = current_shell_state.active_handler->handler(&command);
  
  if (result >= SHELL_RESULT_FAIL) {
		handle_failure(result, command.command);
  } else {
    if (result == SHELL_RESULT_EXIT_MODAL) {
      current_shell_state = (shell_state) {
        .active_handler = &default_handler,
        .modal = false
      };
    }  
    if (result == SHELL_RESULT_ENTER_MODAL) {
      current_shell_state = (shell_state) {
        .active_handler = lookup_handler(command.command),
        .modal = true
      };
    }
  }
  
  return result;
}

static shell_command new_shell_command(char** tokens, uint8_t token_count) {
  shell_command command;
  uint8_t args_offset;
  
  if (current_shell_state.modal) {
    command.command = current_shell_state.active_handler->command;    
    args_offset = 0;
  } else {
    command.command = tokens[0];
    args_offset = 1;
  }
  command.args = &tokens[args_offset];
  command.args_count = token_count - args_offset;
  
  return command;
}

static shell_result delegating_handler(shell_command* command) {
	if (is_handler_registered(command->command)) {
		shell_handler* handler = lookup_handler(command->command);
		return handler->handler(command);    
	} else {
    return SHELL_RESULT_FAIL_MATCH;	  
	}
}

static uint8_t uint8_tokens(const char* cmd_line) {
  uint8_t cmd_length = strlen(cmd_line) + 1;
	char cmd_copy[cmd_length];
  
	strncpy(cmd_copy, cmd_line, cmd_length);
	uint8_t token_count = 0;
	char* state;

	char* token = strtok_r(cmd_copy, " ", &state);
	while (token != NULL) {
		token_count++;
		token = strtok_r(NULL, " ", &state);
	}
  return token_count;
}

static void tokenise(char* cmd_copy, char** tokens, uint8_t tokens_length) {
	uint8_t token_count = 0;
	char* state;

	char* token = strtok_r(cmd_copy, " ", &state);
	while (token != NULL && token_count < tokens_length) {
		tokens[token_count++] = token;
		token = strtok_r(NULL, " ", &state);
	}
}

static void handle_failure(shell_result result, const char* command) {
	if (result == SHELL_RESULT_FAIL_MATCH) {
    PGM_STR(SHELL_UNKNOWN_COMMAND, unknown_cmd)
  	shell_printf(unknown_cmd, command);		
  } else {
    PGM_STR(SHELL_ERROR, error)
		shell_printf(error);
  }
}

