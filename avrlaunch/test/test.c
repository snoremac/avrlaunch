#include <string.h>

#include "unity.h"

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/scheduler.h"
#include "avrlaunch/shell.h"
#include "avrlaunch/pgmspace/pgm_strings.h"

#include "avrlaunch/test/test.h"
#include "avrlaunch/test/buffer_test.h"
#include "avrlaunch/test/buffer_int_test.h"
#include "avrlaunch/test/scheduler_test.h"
#include "avrlaunch/test/shell_test.h"
#include "avrlaunch/test/event_test.h"
#include "avrlaunch/test/buffer_event_test.h"
#include "avrlaunch/test/gpio_event_test.h"

#ifdef SIM
#include <avr/sleep.h>
#endif

static void (*set_up_func)(void);
static void (*tear_down_func)(void);

static shell_result test_shell_handler(shell_command* command);

void setup_task(struct task* task) {
	shell_register_handler("test", test_shell_handler);
	stdout = shell_get_stream();

#ifdef SIM
	test_run();

	cli();
	set_sleep_mode(SLEEP_MODE_IDLE);
	sleep_mode();
#endif

}

void setUp() {
	set_up_func();
}

void tearDown() {
	tear_down_func();
}

uint8_t test_run() {
	UnityBegin();

#ifdef TEST_BUFFER
  PGM_STR(BUFFER_TEST_PATH, buffer_test_path)
	Unity.TestFile = buffer_test_path;
	set_up_func = buffer_test_set_up;
	tear_down_func = buffer_test_tear_down;
	RUN_TEST(test_push, 0);
	RUN_TEST(test_push_overflow, 0);
	RUN_TEST(test_push_no_overflow, 0);
	RUN_TEST(test_no_overflow_shift, 0);
	RUN_TEST(test_unused_elements_are_zero, 0);

  PGM_STR(BUFFER_INT_TEST_PATH, buffer_int_test_path)
	Unity.TestFile = buffer_int_test_path;
	set_up_func = buffer_int_test_set_up;
	tear_down_func = buffer_int_test_tear_down;
	RUN_TEST(test_push_uint8, 0);
	RUN_TEST(test_push_overflow_uint8, 0);
	RUN_TEST(test_push_no_overflow_uint8, 0);
	RUN_TEST(test_no_overflow_shift_uint8, 0);
	RUN_TEST(test_push_uint16, 0);
	RUN_TEST(test_push_overflow_uint16, 0);
	RUN_TEST(test_push_no_overflow_uint16, 0);
	RUN_TEST(test_no_overflow_shift_uint16, 0);

#endif

#ifdef TEST_SCHEDULER
  PGM_STR(SCHEDULER_TEST_PATH, scheduler_test_path)
	Unity.TestFile = scheduler_test_path;
	set_up_func = scheduler_test_set_up;
	tear_down_func = scheduler_test_tear_down;
	RUN_TEST(test_should_add_task, 0);
	RUN_TEST(test_should_invoke_one_shot_task, 0);
	RUN_TEST(test_should_allow_many_one_shot_tasks, 0);
	RUN_TEST(test_should_invoke_repeating_task, 0);
	RUN_TEST(test_should_store_task_data_in_task, 0);
	RUN_TEST(test_should_allow_repeating_tasks_to_cancel_themselves, 0);
	RUN_TEST(test_should_remove_task_by_task_id, 0);
#endif

#ifdef TEST_SHELL
  PGM_STR(SHELL_TEST_PATH, shell_test_path)
	Unity.TestFile = shell_test_path;
	set_up_func = shell_test_set_up;
	tear_down_func = shell_test_tear_down;
	RUN_TEST(test_should_register_handler, 0);
	RUN_TEST(test_should_update_duplicate_handler, 0);
	RUN_TEST(test_should_invoke_handler, 0);
	RUN_TEST(test_should_invoke_handler_with_all_command_tokens, 0);
#endif

#ifdef TEST_EVENT
  PGM_STR(EVENT_TEST_PATH, event_test_path)
	Unity.TestFile = event_test_path;
	set_up_func = event_test_set_up;
	tear_down_func = event_test_tear_down;
	RUN_TEST(test_should_register_event_source, 0);
	RUN_TEST(test_should_update_event_source, 0);
	RUN_TEST(test_should_add_event_listener, 0);
	RUN_TEST(test_should_deregister_event_source, 0);
	RUN_TEST(test_should_remove_event_listeners, 0);
	RUN_TEST(test_should_poll_on_specified_interval, 0);
	RUN_TEST(test_should_invoke_one_event_listener, 0);
	RUN_TEST(test_should_invoke_multiple_event_listeners_for_one_source, 0);
  RUN_TEST(test_should_invoke_event_listeners_for_fired_event, 0);
  RUN_TEST(test_should_invoke_all_category_listeners_when_no_address_specified, 0);
	RUN_TEST(test_should_invoke_event_listener_repeatedly, 0);
	RUN_TEST(test_should_allow_event_listeners_to_remove_themselves, 0);
	RUN_TEST(test_should_deregister_event_source_when_event_listener_removed, 0);
#endif

#ifdef TEST_BUFFER_EVENT
  PGM_STR(BUFFER_EVENT_TEST_PATH, buffer_event_test_path)
	Unity.TestFile = buffer_event_test_path;
	set_up_func = buffer_event_test_set_up;
	tear_down_func = buffer_event_test_tear_down;
	RUN_TEST(test_should_register_buffer_event_source_when_event_listener_added, 0);
	RUN_TEST(test_should_raise_single_event_on_buffer_full, 0);
	RUN_TEST(test_should_raise_single_event_on_buffer_not_full, 0);
#endif

#ifdef TEST_GPIO_EVENT
  PGM_STR(GPIO_EVENT_TEST_PATH, gpio_event_test_path)
	Unity.TestFile = gpio_event_test_path;
	set_up_func = gpio_event_test_set_up;
	tear_down_func = gpio_event_test_tear_down;
	RUN_TEST(test_should_register_event_source_when_event_listener_added, 0);
	#ifdef SIM
	RUN_TEST(test_should_raise_event_after_logic_level_change, 0);
	#endif
	RUN_TEST(test_not_should_raise_without_logic_level_change, 0);
	RUN_TEST(test_gpio_descriptor, 0);
#endif

	return UnityEnd();
}

static shell_result test_shell_handler(shell_command* command) {
	if (strcmp(command->command, "test") == 0) {
		test_run();
		return SHELL_RESULT_SUCCESS;
	}
	return SHELL_RESULT_FAIL;
}
