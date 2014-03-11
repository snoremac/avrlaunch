#ifndef AVRLAUNCH_TEST_H
#define AVRLAUNCH_TEST_H

#include <inttypes.h>

extern void setUp(void);
extern void tearDown(void);

void test_init();

uint8_t test_run();

#endif
