#ifndef AVRLAUNCH_SCHEDULER_H
#define AVRLAUNCH_SCHEDULER_H

#include <stdbool.h>
#include <inttypes.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/clock.h"

#ifndef SCHEDULER_MAX_TASKS
#define SCHEDULER_MAX_TASKS 1
#endif

#define TASK_ONCE 1
#define TASK_FOREVER 0
#define TASK_ASAP 0
#define TASK_NO_TASK 0

struct task;
  
typedef void (*task_callback)(struct task* task);

struct task_config {
  char name[5];
	uint8_t times;	
	time interval_time;
};

struct task {
  bool active;
	uint8_t id;
  struct task_config config;
	uint8_t times_run;	
	time next_scheduled_time;
  task_callback task;
  void* data;
};

void scheduler_init(void);

void scheduler_tick(void);

uint8_t scheduler_add_task(struct task_config* config, task_callback task, void* data);

result scheduler_remove_task(uint8_t id);

void scheduler_clear_tasks(void);

void setup_task(struct task* task);

#endif
