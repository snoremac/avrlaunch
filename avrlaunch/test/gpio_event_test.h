#ifndef AVRLAUNCH_GPIO_EVENT_TEST_H
#define AVRLAUNCH_GPIO_EVENT_TEST_H

void gpio_event_test_set_up(void);
void gpio_event_test_tear_down(void);

void test_should_register_event_source_when_event_listener_added(void);
void test_should_raise_event_after_logic_level_change(void);
void test_not_should_raise_without_logic_level_change(void);
void test_gpio_descriptor(void);

#endif
