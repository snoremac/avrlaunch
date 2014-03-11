
#include <stdio.h>
#include <avr/io.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/scheduler.h"
#include "avrlaunch/shell.h"
#include "avrlaunch/event/gpio_event.h"
#include "avrlaunch/hal/hal_gpio.h"

static gpio led_gpio;
static gpio button_gpio;

static bool light_on;
static uint8_t led_task_id;

static void start_blink();
static void stop_blink();

static bool button_event_handler(event* event);
static void flip_led_task(struct task* task);

static shell_result blink_shell_handler(shell_command* command);

void setup_task(struct task* task) {
	led_gpio = GPIO(PORTD, PIN6);
	button_gpio = GPIO(PORTD, PIN2);

	gpio_set_input(&button_gpio);
	gpio_set_output(&led_gpio);

	gpio_event_add_listener(&button_gpio, button_event_handler);
	shell_register_handler("blink", blink_shell_handler);	
	start_blink();
}

static void start_blink() {
	gpio_write_high(&led_gpio);
	led_task_id = scheduler_add_task(&(struct task_config){"led", TASK_FOREVER, 500}, flip_led_task, NULL);
	light_on = true;				  
}

static void stop_blink() {
	scheduler_remove_task(led_task_id);	
	gpio_write_low(&led_gpio);
	light_on = false;				  
}

static bool button_event_handler(event* event) {
	gpio_event* button_event = (gpio_event*) event;
	if (button_event->super.descriptor == gpio_to_descriptor(&button_gpio)) {
		if (button_event->event_type == GPIO_DOWN) {
			if (light_on) {
        stop_blink();
			} else {
        start_blink();
			}
		}
	}
	return true;
}

static void flip_led_task(struct task* task) {
	gpio_toggle(&led_gpio);
}

static shell_result blink_shell_handler(shell_command* command) {
	if (command->args_count == 0) return SHELL_RESULT_FAIL;
	
	if (string_eq(command->command, "blink")) {
		if (string_eq(command->args[0], "start")) {
			start_blink();
		} else if (string_eq(command->args[0], "stop")) {
			stop_blink();
		} else {
			return SHELL_RESULT_FAIL;
		}
	}
	return SHELL_RESULT_SUCCESS;
}
