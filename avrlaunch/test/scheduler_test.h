#ifndef AVRLAUNCH_SCHEDULER_TEST_H
#define AVRLAUNCH_SCHEDULER_TEST_H

#include <inttypes.h>

void scheduler_test_set_up(void);
void scheduler_test_tear_down(void);

void test_should_add_task(void);
void test_should_invoke_one_shot_task(void);
void test_should_allow_many_one_shot_tasks(void);
void test_should_invoke_repeating_task(void);
void test_should_store_task_data_in_task(void);
void test_should_allow_repeating_tasks_to_cancel_themselves(void);
void test_should_remove_task_by_task_id(void);

#endif
