#ifndef AVRLAUNCH_EVENT_TEST_H
#define AVRLAUNCH_EVENT_TEST_H

#include "avrlaunch/event/event.h"

typedef struct test_event {
	event super;
	uint8_t value;
} test_event;

void event_test_set_up(void);
void event_test_tear_down(void);

void test_should_register_event_source(void);
void test_should_update_event_source(void);
void test_should_add_event_listener(void);
void test_should_deregister_event_source(void);
void test_should_remove_event_listeners(void);
void test_should_poll_on_specified_interval(void);
void test_should_invoke_one_event_listener(void);
void test_should_invoke_multiple_event_listeners_for_one_source(void);
void test_should_invoke_event_listeners_for_fired_event();
void test_should_invoke_all_category_listeners_when_no_address_specified(void);
void test_should_invoke_event_listener_repeatedly(void);
void test_should_allow_event_listeners_to_remove_themselves(void);
void test_should_deregister_event_source_when_event_listener_removed(void);

#endif
