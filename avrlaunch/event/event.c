
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/clock.h"
#include "avrlaunch/log.h"
#include "avrlaunch/shell.h"
#include "avrlaunch/pgmspace/pgm_strings.h"
#include "avrlaunch/event/event.h"
#include "avrlaunch/event/event_internal.h"

#define SHELL_SUB_HANDLERS_COUNT 1

typedef shell_result (*shell_sub_handler)(char** command_tokens, uint8_t token_count);

static event_source sources[EVENT_MAX_SOURCES];
static event_listener listeners[EVENT_MAX_LISTENERS];

static bool event_matches_listener(event* event, event_listener* listener);
static bool is_same_source(event_source* source_a, event_type type_b, event_descriptor descriptor_b);

static bool source_registered(event_type type, event_descriptor descriptor);
static result update_source(event_type type, event_descriptor descriptor, event_poll_handler poll_handler);
static void warn_failed_add_source(event_type type, event_descriptor descriptor, result result);

static void warn_failed_add_listener(event_type type, event_descriptor descriptor, result result);

static shell_result shell_main_handler(shell_command* command);
static shell_result shell_list_handler(char** args, uint8_t token_count);
static shell_result shell_list_sources();
static shell_result shell_list_listeners();

static shell_sub_handler shell_sub_handlers[SHELL_SUB_HANDLERS_COUNT] = {
	shell_list_handler
};

void event_init() {
  PGM_STR(EVENT_SHELL_EVENT, shell_event)
  shell_register_handler(shell_event, shell_main_handler);
}

void event_tick() {
	time time_now = clock_get_time();

	for (uint8_t i = 0; i < EVENT_MAX_SOURCES; i++) {
		if (!sources[i].super.active) continue;
		
		if (time_now >= sources[i].next_scheduled_poll) {
      event* event = sources[i].poll_handler(sources[i].super.type, sources[i].super.descriptor);		  
  		if (null(event)) continue;  
      event_fire_event(event);

    	sources[i].next_scheduled_poll = time_now + sources[i].poll_interval;	
		}
	}
}

void event_fire_event(event* event) {
	for (uint8_t i = 0; i < EVENT_MAX_LISTENERS; i++) {
		if (!listeners[i].super.active) continue;

		if (event_matches_listener(event, &listeners[i])) {
			bool keep_listening = listeners[i].handler(event);
			if (!keep_listening) {
  			listeners[i].super.active = false;			  
			}
		}		
	}
}

static bool event_matches_listener(event* event, event_listener* listener) {
	return event != NULL && listener != NULL
		&& event->type == listener->super.type && event->descriptor == listener->super.descriptor;
}

result event_register_source(event_type type, event_descriptor descriptor,
    time poll_interval, event_poll_handler poll_handler) {
	if (source_registered(type, descriptor)) {
		update_source(type, descriptor, poll_handler);
		return RESULT_SUCCESS;
	}

	for (uint8_t i = 0; i < EVENT_MAX_SOURCES; i++) {    
		if (!sources[i].super.active) {
			sources[i] = (event_source) {
			  {
          .active = true,
				  .type = type,
				  .descriptor = descriptor
			  },
        .poll_interval = poll_interval,
        .next_scheduled_poll = clock_get_time() + poll_interval,
			  .poll_handler = poll_handler
		  };
			return RESULT_SUCCESS;
    }
	}

	warn_failed_add_source(type, descriptor, RESULT_FAIL_CAPACITY);
	return RESULT_FAIL_CAPACITY;
}

void event_deregister_source(event_type type, event_descriptor descriptor) {
	for (uint8_t i = 0; i < EVENT_MAX_SOURCES; i++) {
		if (is_same_source(&sources[i], type, descriptor)) {
			sources[i].super.active = false;
		}
	}	
}

static bool source_registered(event_type type, event_descriptor descriptor) {
	for (uint8_t i = 0; i < EVENT_MAX_SOURCES; i++) {
		if (is_same_source(&sources[i], type, descriptor)) {
			return true;
		}
	}	
	return false;
}

static bool is_same_source(event_source* source_a, event_type type_b, event_descriptor descriptor_b) {
	return (source_a != NULL) && (source_a->super.active) && (source_a->super.type == type_b) && (source_a->super.descriptor == descriptor_b);
}

static result update_source(event_type type, event_descriptor descriptor, event_poll_handler poll_handler) {
	for (uint8_t i = 0; i < EVENT_MAX_SOURCES; i++) {
		if (is_same_source(&sources[i], type, descriptor)) {
			sources[i].poll_handler = poll_handler;
			return RESULT_SUCCESS;
		}
	}	
	return RESULT_FAIL_MATCH;
}

static void warn_failed_add_source(event_type type, event_descriptor descriptor, result result) {
  set_system_warning();
	if (LOG_LEVEL >= LOG_LEVEL_WARN) {
    PGM_STR(EVENT_SOURCE_FAILED_ADD, failed_msg)
		LOG_WARN(failed_msg, type, descriptor, result);
	}
}

result event_add_listener(event_type type, event_descriptor descriptor, event_handler handler) {
	for (uint8_t i = 0; i < EVENT_MAX_LISTENERS; i++) {
		if (!listeners[i].super.active) {
			listeners[i] = (event_listener) {
			  {
          .active = true,
				  .type = type,
				  .descriptor = descriptor				
			  },
			  .handler = handler
		  };
			return RESULT_SUCCESS;
		}		
	}
  
	warn_failed_add_listener(type, descriptor, RESULT_FAIL_CAPACITY);
	return RESULT_FAIL_CAPACITY;
}

static void warn_failed_add_listener(event_type type, event_descriptor descriptor, result result) {
  set_system_warning();
	if (LOG_LEVEL >= LOG_LEVEL_WARN) {
    PGM_STR(EVENT_LISTENER_FAILED_ADD, failed_msg)
    LOG_WARN(failed_msg, type, descriptor, result);
	}	
}

void event_remove_listeners(event_type type, event_descriptor descriptor) {
	for (uint8_t i = 0; i < EVENT_MAX_LISTENERS; i++) {
		if (listeners[i].super.active && listeners[i].super.type == type && listeners[i].super.descriptor == descriptor) {
			listeners[i].super.active = false;
		}		
	}
}

void event_clear_sources() {
	for (uint8_t i = 0; i < EVENT_MAX_SOURCES; i++) {
		if (sources[i].super.active) {
			sources[i].super.active = false;
		}		
	}
}

void event_clear_listeners() {
	for (uint8_t i = 0; i < EVENT_MAX_LISTENERS; i++) {
		if (listeners[i].super.active) {
			listeners[i].super.active = false;
		}		
	}
}

uint8_t event_source_count() {
	uint8_t count = 0;
	for (uint8_t i = 0; i < EVENT_MAX_SOURCES; i++) {
		if (sources[i].super.active) {
			count++;
		}		
	}
	return count;
}

uint8_t event_listener_count() {
	uint8_t count = 0;
	for (uint8_t i = 0; i < EVENT_MAX_LISTENERS; i++) {
		if (listeners[i].super.active) {
			count++;
		}		
	}
	return count;
}

event_source* first_event_source() {
	for (uint8_t i = 0; i < EVENT_MAX_SOURCES; i++) {
		if (sources[i].super.active) {
			return &sources[i];
		}		
	}
	return NULL;
}

event_listener* first_event_listener() {
	for (uint8_t i = 0; i < EVENT_MAX_LISTENERS; i++) {
		if (listeners[i].super.active) {
			return &listeners[i];
		}		
	}
	return NULL;
}

static shell_result shell_main_handler(shell_command* command) {
  PGM_STR(EVENT_SHELL_EVENT, shell_event)
    
	result result = SHELL_RESULT_FAIL;
	if (strcmp(command->command, shell_event) == 0) {
		for (uint8_t i = 0; i < SHELL_SUB_HANDLERS_COUNT && result != RESULT_SUCCESS; i++) {
			result = shell_sub_handlers[i](command->args, command->args_count);
		}
	}
	return result;	
}

static shell_result shell_list_handler(char** args, uint8_t args_count) {
  PGM_STR(EVENT_SHELL_LIST, shell_list)
  PGM_STR(EVENT_SHELL_SRC, shell_src)
  PGM_STR(EVENT_SHELL_LNR, shell_lnr)
    
	if (!string_eq(args[0], shell_list) || args_count < 2) return SHELL_RESULT_FAIL;	

	if (string_eq(args[1], shell_src)) {
		return shell_list_sources();
	} else if (string_eq(args[1], shell_lnr)) {
		return shell_list_listeners();
	} else {
		return SHELL_RESULT_FAIL;		
	}				
}

static shell_result shell_list_sources() {
  PGM_STR(EVENT_SHELL_SOURCE_LIST_HEADER, list_header)
	shell_printf(list_header);

	if (event_source_count() == 0) {
    PGM_STR(EVENT_SHELL_SOURCE_EMPTY_ROW, empty_row)
		shell_printf(empty_row);		
		return SHELL_RESULT_SUCCESS;
	}

	uint8_t count = 0;
  PGM_STR(EVENT_SHELL_SOURCE_ROW, row)
	for (uint8_t i = 0; i < EVENT_MAX_SOURCES; i++) {
		if (sources[i].super.active) {
			shell_printf(row, ++count, i, sources[i].super.type, sources[i].super.descriptor, sources[i].poll_interval);
		}
	}
	return SHELL_RESULT_SUCCESS;
}

static shell_result shell_list_listeners() {
  PGM_STR(EVENT_SHELL_LISTENER_LIST_HEADER, list_header)
	shell_printf(list_header);

	if (event_listener_count() == 0) {
    PGM_STR(EVENT_SHELL_LISTENER_EMPTY_ROW, empty_row)
		shell_printf(empty_row);		
		return SHELL_RESULT_SUCCESS;
	}

	uint8_t count = 0;
  PGM_STR(EVENT_SHELL_LISTENER_ROW, row)
	for (uint8_t i = 0; i < EVENT_MAX_LISTENERS; i++) {
		if (listeners[i].super.active) {
			shell_printf(row, ++count, i, listeners[i].super.type, listeners[i].super.descriptor);
		}
	}
	return SHELL_RESULT_SUCCESS;
}
