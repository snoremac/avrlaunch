#include <stdbool.h>
#include <string.h>

#include "unity.h"

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/clock.h"
#include "avrlaunch/scheduler.h"
#include "avrlaunch/scheduler_internal.h"
#include "avrlaunch/shell.h"

#include "avrlaunch/test/scheduler_test.h"

static volatile time callback_time;
static volatile time on_complete_time;
static volatile uint8_t callback_count;
static volatile uint8_t on_complete_count;
static char callback_data[5];

static void noop_task(struct task* task);
static void counting_task(struct task* task);
static void cancelling_task(struct task* task);
static void data_check_task(struct task* task);

void scheduler_test_set_up() {
	scheduler_clear_tasks();
	callback_time = 0;
	callback_count = 0;
  on_complete_count = 0;
	memset(callback_data, '\0', ARRAY_SIZE(callback_data));
}

void scheduler_test_tear_down() {
	scheduler_clear_tasks();
}

void test_should_add_task() {
  struct task_config config = { "noop", TASK_ONCE, 1000 };
	uint8_t task_id = scheduler_add_task(&config, noop_task, NULL);
	TEST_ASSERT_NOT_EQUAL(TASK_NO_TASK, task_id);
	TEST_ASSERT_EQUAL_UINT(1, scheduler_task_count());
	TEST_ASSERT_EQUAL_STRING("noop", scheduler_first_task()->config.name);
	TEST_ASSERT_EQUAL_PTR(noop_task, scheduler_first_task()->task);
}

void test_should_invoke_one_shot_task() {
	time start_time = clock_get_time();
  struct task_config config = { "noop", TASK_ONCE, 1000 };
	scheduler_add_task(&config, counting_task, NULL);

	clock_add_time(1000);
	scheduler_tick();
	clock_add_time(2000);
	scheduler_tick();

	TEST_ASSERT_EQUAL_UINT(1, callback_count);
	TEST_ASSERT_UINT_WITHIN(100, start_time + 1000, callback_time);             
}

void test_should_allow_many_one_shot_tasks() {
	for (uint32_t i = 0; i < 50; i++) {
    struct task_config config = { "once", TASK_ONCE, 1000 };
		scheduler_add_task(&config, counting_task, NULL);

		clock_add_time((i + 1) * 1000);
		scheduler_tick();
	}
	
	TEST_ASSERT_EQUAL_UINT(50, callback_count);
}

void test_should_invoke_repeating_task() {
  struct task_config config = { "count", TASK_FOREVER, 1000 };
	scheduler_add_task(&config, counting_task, NULL);
	
	for (uint32_t i = 0; i < 10; i++) {
		clock_add_time((i + 1) * 1000);
		scheduler_tick();
	}

	TEST_ASSERT_EQUAL_UINT(10, callback_count);
}

void test_should_allow_repeating_tasks_to_cancel_themselves() {
  struct task_config config = { "count", TASK_FOREVER, 1000 };
	scheduler_add_task(&config, cancelling_task, NULL);
	
	for (uint32_t i = 0; i < 10; i++) {
		clock_add_time((i + 1) * 1000);
		scheduler_tick();
	}

	TEST_ASSERT_EQUAL_UINT(5, callback_count);
}

void test_should_store_task_data_in_task() {
  struct task_config config = { "once", TASK_ONCE, 1000 };
	scheduler_add_task(&config, data_check_task, "data");

	clock_add_time(1000);
	scheduler_tick();

	TEST_ASSERT_EQUAL_STRING("data", callback_data);
}

void test_should_remove_task_by_task_id() {
  struct task_config noop_config = { "noop", TASK_FOREVER, 1000 };
	uint8_t noop_id = scheduler_add_task(&noop_config, noop_task, NULL);
  struct task_config count_config = { "count", TASK_FOREVER, 1000 };
	uint8_t count_id = scheduler_add_task(&count_config, counting_task, NULL);
	TEST_ASSERT_EQUAL_UINT(2, scheduler_task_count());
	
	result result = scheduler_remove_task(noop_id);
	TEST_ASSERT_EQUAL_UINT(RESULT_SUCCESS, result);
	TEST_ASSERT_EQUAL_UINT(1, scheduler_task_count());
	TEST_ASSERT_EQUAL_UINT(count_id, scheduler_first_task()->id);

	result = scheduler_remove_task(noop_id);
	TEST_ASSERT_EQUAL_UINT(RESULT_FAIL_MATCH, result);
}

static void noop_task(struct task* task) {
}

static void counting_task(struct task* task) {
	callback_time = clock_get_time();
	callback_count++;
}

static void cancelling_task(struct task* task) {
  if (++callback_count == 5) {
    scheduler_remove_task(task->id);
  }
}

static void data_check_task(struct task* task) {
  char* task_data = (char*) task->data;
  strncpy(callback_data, task_data, ARRAY_SIZE(callback_data));
}
