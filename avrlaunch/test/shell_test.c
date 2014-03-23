#include <stdbool.h>
#include <string.h>

#include "unity.h"

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/shell.h"
#include "avrlaunch/shell_internal.h"
#include "avrlaunch/pgmspace/pgm_strings.h"

#include "avrlaunch/test/shell_test.h"

#define TOKEN_MAX_LENGTH 10

static uint8_t handler_count;
static uint8_t last_command_arg_count;
static char last_command[TOKEN_MAX_LENGTH];
static char last_command_arg_1[TOKEN_MAX_LENGTH];
static char last_command_arg_2[TOKEN_MAX_LENGTH];

static shell_result shell_handler_1(shell_command* command);
static shell_result shell_handler_2(shell_command* command);

void shell_test_set_up() {
	shell_clear_handlers();
	handler_count = 0;
}

void shell_test_tear_down() {
	shell_clear_handlers();
}

void test_should_register_handler() {
  PGM_STR(CMD, cmd)
	shell_register_handler(cmd, shell_handler_1);
	TEST_ASSERT_EQUAL_UINT(1, shell_handler_count());
	TEST_ASSERT_EQUAL_PTR(shell_handler_1, shell_first_handler()->handler);
}

void test_should_update_duplicate_handler() {
  PGM_STR(CMD, cmd)
	result result = shell_register_handler(cmd, shell_handler_1);
	TEST_ASSERT_EQUAL_UINT(SHELL_RESULT_SUCCESS, result);
	TEST_ASSERT_EQUAL_UINT(1, shell_handler_count());

	result = shell_register_handler(cmd, shell_handler_2);
	TEST_ASSERT_EQUAL_UINT(SHELL_RESULT_SUCCESS, result);
	TEST_ASSERT_EQUAL_UINT(1, shell_handler_count());
	TEST_ASSERT_EQUAL_PTR(shell_handler_2, shell_first_handler()->handler);
}

void test_should_invoke_handler() {
  PGM_STR(UPTIME, uptime)
	shell_register_handler(uptime, shell_handler_1);
	shell_invoke(uptime);
	TEST_ASSERT_EQUAL_UINT(0, last_command_arg_count);	
	TEST_ASSERT_EQUAL_STRING(uptime, last_command);
}

void test_should_invoke_handler_with_all_command_tokens() {
  PGM_STR(CMD, cmd)
  PGM_STR(CMD_DO_IT, cmd_do_it)
	shell_register_handler(cmd, shell_handler_1);
	shell_invoke(cmd_do_it);
	TEST_ASSERT_EQUAL_UINT(2, last_command_arg_count);	
	TEST_ASSERT_EQUAL_STRING(cmd, last_command);
	TEST_ASSERT_EQUAL_STRING("do", last_command_arg_1);
	TEST_ASSERT_EQUAL_STRING("it", last_command_arg_2);
}

static shell_result shell_handler_1(shell_command* command) {
	last_command_arg_count = command->args_count;
	strncpy(last_command, command->command, TOKEN_MAX_LENGTH);
	if (command->args_count > 0) {
		strncpy(last_command_arg_1, command->args[0], TOKEN_MAX_LENGTH);
	}
	if (command->args_count > 1) {
		strncpy(last_command_arg_2, command->args[1], TOKEN_MAX_LENGTH);
	}
	return SHELL_RESULT_SUCCESS;
}

static shell_result shell_handler_2(shell_command* command) {
	return SHELL_RESULT_SUCCESS;	
}
