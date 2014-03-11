#ifndef AVRLAUNCH_SCHEDULER_INTERNAL_H
#define AVRLAUNCH_SCHEDULER_INTERNAL_H

#include <inttypes.h>
#include <stdbool.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/scheduler.h"

uint8_t scheduler_task_count(void);

struct task* scheduler_first_task(void);

#endif
