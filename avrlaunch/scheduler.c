#include <string.h>
#include <stdbool.h>
#include <avr/pgmspace.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/clock.h"
#include "avrlaunch/log.h"
#include "avrlaunch/scheduler.h"
#include "avrlaunch/scheduler_internal.h"
#include "avrlaunch/shell.h"
#include "avrlaunch/pgmspace/pgm_strings.h"

#define SHELL_SUB_HANDLERS_COUNT 1

typedef shell_result (*shell_sub_handler)(char** command_tokens, uint8_t token_count);

static struct task tasks[SCHEDULER_MAX_TASKS];

static bool is_task_runnable(struct task* task, time time_now);
static bool is_task_complete(struct task* task);
static void reschedule_task(struct task* task, time last_scheduled_time);
static void warn_failed_add_task(result result);

static shell_result shell_main_handler(shell_command* command);
static shell_result shell_list_handler(char** args, uint8_t token_count);
static void print_list_header(void);

static shell_sub_handler shell_sub_handlers[SHELL_SUB_HANDLERS_COUNT] = {
	shell_list_handler
};

void scheduler_init() {
  PGM_STR(SCHEDULER_SHELL_SCHED, shell_sched)
  shell_register_handler(shell_sched, shell_main_handler);
}

void scheduler_tick() {
	time time_now = clock_get_time();
	for (uint8_t i = 0; i < SCHEDULER_MAX_TASKS; i++) {
		
		if (is_task_runnable(&tasks[i], time_now)) {
			tasks[i].times_run++;
			tasks[i].task(&tasks[i]);

			if (is_task_complete(&tasks[i])) {
				tasks[i].active = false;
			} else {
				reschedule_task(&tasks[i], time_now);
			}
		}
	}
}

static bool is_task_runnable(struct task* task, time time_now) {
	return (task != NULL) && (task->active) && (time_now >= task->next_scheduled_time);
}

static bool is_task_complete(struct task* task) {
	return (task->times_run == task->config.times) && (task->config.times != TASK_FOREVER);
}

static void reschedule_task(struct task* task, time last_scheduled_time) {
	task->next_scheduled_time = last_scheduled_time + task->config.interval_time;	
}

uint8_t scheduler_add_task(struct task_config* config, task_callback task, void* data) {
	for (uint8_t i = 0; i < SCHEDULER_MAX_TASKS; i++) {
		if (tasks[i].active) continue;
		
		tasks[i].active = true;
    tasks[i].id = i + 1;
		strncpy(tasks[i].config.name, config->name, ARRAY_SIZE(tasks[i].config.name));
    tasks[i].config.times = config->times;
    tasks[i].config.interval_time = config->interval_time;
    tasks[i].times_run = 0;
    tasks[i].next_scheduled_time = clock_get_time() + config->interval_time;
    tasks[i].task = task;
    tasks[i].data = data;
		return tasks[i].id;
	}
	
	warn_failed_add_task(RESULT_FAIL_CAPACITY);
	return TASK_NO_TASK;
}

static void warn_failed_add_task(result result) {
  set_system_warning();
	if (LOG_LEVEL >= LOG_LEVEL_WARN) {
		PGM_STR(SCHEDULER_FAILED_ADD, failed_msg);
		LOG_WARN(failed_msg, result);
	}
}

result scheduler_remove_task(uint8_t task_id) {
  if (tasks[task_id - 1].active) {
		tasks[task_id - 1].active = false;
    return RESULT_SUCCESS;
	}	else {
    return RESULT_FAIL_MATCH;
	}
}

void scheduler_clear_tasks() {
	for (uint8_t i = 0; i < SCHEDULER_MAX_TASKS; i++) {
		tasks[i].active = false;
	}	
}

uint8_t scheduler_task_count() {
	uint8_t count = 0;
	for (uint8_t i = 0; i < SCHEDULER_MAX_TASKS; i++) {
		if (tasks[i].active) {
			count++;
		}
	}	
	return count;
}

struct task* scheduler_first_task() {
	for (uint8_t i = 0; i < SCHEDULER_MAX_TASKS; i++) {
		if (tasks[i].active) {
			return &tasks[i];
		}
	}	
	return NULL;
}

static shell_result shell_main_handler(shell_command* command) {
	PGM_STR(SCHEDULER_SHELL_SCHED, sched);
	shell_result result = SHELL_RESULT_FAIL;
	if (strcmp(command->command, sched) == 0) {
		for (uint8_t i = 0; i < SHELL_SUB_HANDLERS_COUNT && result != SHELL_RESULT_SUCCESS; i++) {
			result = shell_sub_handlers[i](command->args, command->args_count);
		}
	}
	return result;	
}

static shell_result shell_list_handler(char** args, uint8_t args_count) {
	PGM_STR(SCHEDULER_SHELL_LIST, list);
	PGM_STR(SCHEDULER_SHELL_ROW, row);
	PGM_STR(SCHEDULER_SHELL_EMPTY_ROW, empty_row);
  
	if (strcmp(args[0], list) != 0) {
		return SHELL_RESULT_FAIL;	
	}

	print_list_header();
	
	if (scheduler_task_count() == 0) {
		shell_printf(empty_row);
		return SHELL_RESULT_SUCCESS;
	}
	
	uint8_t count = 0;
	for (uint8_t i = 0; i < SCHEDULER_MAX_TASKS; i++) {
		if (tasks[i].active) {
			shell_printf(row, ++count, tasks[i].id, tasks[i].config.name, tasks[i].config.times, tasks[i].times_run, tasks[i].config.interval_time);
		}
	}
					
	return SHELL_RESULT_SUCCESS;
}

static void print_list_header() {
	PGM_STR(SCHEDULER_LIST_HEADER, list_header);
	shell_printf(list_header);
}

