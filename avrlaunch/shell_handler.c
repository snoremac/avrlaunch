#include <inttypes.h>
#include <string.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/log.h"
#include "avrlaunch/shell.h"
#include "avrlaunch/shell_internal.h"
#include "avrlaunch/pgmspace/pgm_strings.h"

static shell_handler handlers[SHELL_MAX_HANDLERS];

static result update_handler(const char* command, shell_result (*handler)(shell_command* command));
static void warn_failed_add_handler(result result);

void shell_handler_init() {
}

result shell_register_handler(const char* command, shell_result (*handler)(shell_command* command)) {
	if (is_handler_registered(command)) {
		update_handler(command, handler);
		return RESULT_SUCCESS;
	}

	for (uint8_t i = 0; i < SHELL_MAX_HANDLERS; i++) {
		if (!handlers[i].active) {      
  		strlcpy(handlers[i].command, command, SHELL_TOKEN_MAX_LENGTH);
  		handlers[i].handler = handler;
      handlers[i].active = true;
  		return RESULT_SUCCESS;
    }
	}
	warn_failed_add_handler(RESULT_FAIL_CAPACITY);
	return RESULT_FAIL_CAPACITY;
}

bool is_handler_registered(const char* command) {
	for (int i = 0; i < SHELL_MAX_HANDLERS; i++) {
		if (handlers[i].active && string_eq(handlers[i].command, command)) {
			return true;
		}				
	}
	return false;
}

shell_handler* lookup_handler(const char* command) {
	for (int i = 0; i < SHELL_MAX_HANDLERS; i++) {
		if (handlers[i].active && string_eq(handlers[i].command, command)) {
			return &handlers[i];
		}				
	}
	return NULL;
}

static result update_handler(const char* command, shell_result (*handler)(shell_command* command)) {
	for (int i = 0; i < SHELL_MAX_HANDLERS; i++) {
		if (handlers[i].active && string_eq(handlers[i].command, command)) {
			handlers[i].handler = handler;
			return RESULT_SUCCESS;
		}				
	}
	return RESULT_FAIL_MATCH;
}

static void warn_failed_add_handler(result result) {
  set_system_warning();
  PGM_STR(SHELL_FAILED_ADD_HANDLER, failed_msg)
	if (LOG_LEVEL >= LOG_LEVEL_WARN) {
		LOG_WARN(failed_msg, result);
	}
}

void shell_clear_handlers() {
	for (int i = 0; i < SHELL_MAX_HANDLERS; i++) {
		if (handlers[i].active) {
      handlers[i].active = false;
		}
	}
}

uint8_t shell_handler_count(void) {
	uint8_t count = 0;
	for (int i = 0; i < SHELL_MAX_HANDLERS; i++) {
		if (handlers[i].active) {
			count++;
		}
	}
	return count;
}

shell_handler* shell_first_handler(void) {
	for (int i = 0; i < SHELL_MAX_HANDLERS; i++) {
		if (handlers[i].active) {
			return &handlers[i];
		}
	}
	return NULL;
}


