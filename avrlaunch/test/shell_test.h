#ifndef AVRLAUNCH_SHELL_TEST_H
#define AVRLAUNCH_SHELL_TEST_H

#include <inttypes.h>

void shell_test_set_up(void);
void shell_test_tear_down(void);

void test_should_register_handler(void);
void test_should_update_duplicate_handler(void);
void test_should_invoke_handler(void);
void test_should_invoke_handler_with_all_command_tokens(void);

#endif
