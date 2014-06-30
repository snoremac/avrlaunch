
#include <stdio.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/log.h"
#include "avrlaunch/scheduler.h"
#include "avrlaunch/shell.h"
#include "avrlaunch/event/adc_event.h"
#include "avrlaunch/event/gpio_event.h"
#include "avrlaunch/hal/hal_gpio.h"

static gpio led_gpio;
static gpio_timer led_gpio_timer;
static gpio button_gpio;

static bool fade_on;
static uint8_t brightness;

void configure_timer(void);
static void start_fade(void);
static void stop_fade(void);
static void start_fade_up(void);
static void start_fade_down(void);
static void set_brightness(uint8_t brightness);

static bool button_event_handler(event* event); 

static void brightness_up_task(struct task* task);
static void brightness_down_task(struct task* task);
static void light_off_task(struct task* task);

static shell_result fade_shell_handler(shell_command* command);

uint8_t brightness_up_task_id;
uint8_t brightness_down_task_id;
uint8_t light_off_task_id;

void setup_task(struct task* task) {
	led_gpio = GPIO(PORTD, PIN6);
#if defined (__AVR_ATmega328P__) || defined (__AVR_ATmega328__)
	led_gpio_timer = GPIO_TIMER(PORTD, PIN6);
#elif defined (__AVR_ATmega32U4__) || defined (__AVR_ATmega32U4__)
	led_gpio_timer = GPIO_TIMER(PORTD, PIN0);
#endif	
	button_gpio = GPIO(PORTD, PIN2);

	configure_timer();

	gpio_set_output(&led_gpio);		
	gpio_connect_timer_non_inverting(&led_gpio_timer);
	gpio_set_output_compare_percentage(&led_gpio_timer, 0);

	gpio_event_add_listener(&button_gpio, button_event_handler);

	shell_register_handler("fade", fade_shell_handler);
	start_fade();
}

void configure_timer() {
  // Timer 0
  // Prescale 64
  set_bit(&TCCR0B, CS00);
  set_bit(&TCCR0B, CS01);
  clear_bit(&TCCR0B, CS02);

  // Fast PWM, TOP at 0xFF
  set_bit(&TCCR0A, WGM00);
  set_bit(&TCCR0A, WGM01);
  clear_bit(&TCCR0B, WGM02);
}

static void start_fade() {
	start_fade_up();
	fade_on = true;
}

static void stop_fade() {
  struct task_config config = { "loff", TASK_FOREVER, 20 };
	light_off_task_id = scheduler_add_task(&config, light_off_task, NULL);
	fade_on = false;
}

static void start_fade_up() {
  struct task_config config = { "brup", TASK_FOREVER, 20 };
	brightness_up_task_id = scheduler_add_task(&config, brightness_up_task, NULL);
}

static void start_fade_down() {
  struct task_config config = { "brdn", TASK_FOREVER, 20 };
	brightness_down_task_id = scheduler_add_task(&config, brightness_down_task, NULL);
}

static void set_brightness(uint8_t brightness) {
	gpio_set_output_compare_percentage(&led_gpio_timer, brightness);			
}

static void brightness_up_task(struct task* task) {
	brightness += 5;
	set_brightness(brightness);

	if (brightness == 100) {
		scheduler_remove_task(task->id);
		start_fade_down();
	}
}

static void brightness_down_task(struct task* task) {
	brightness -= 5;
	set_brightness(brightness);

	if (brightness == 0) {
		scheduler_remove_task(task->id);
		start_fade_up();
	}
}

static void light_off_task(struct task* task) {
	if (brightness <= 5) {
		set_brightness(0);		
		scheduler_remove_task(brightness_up_task_id);
		scheduler_remove_task(brightness_down_task_id);
		scheduler_remove_task(task->id);
	}
}

static bool button_event_handler(event* event) {
	if (gpio_event_is_from(event, &button_gpio)) {
		if (gpio_event_is_down(event)) {
			if (fade_on) {
				stop_fade();
			} else {
				start_fade();
			}
		}
	}
	return true;
}

static shell_result fade_shell_handler(shell_command* command) {
	if (command->args_count == 0) return SHELL_RESULT_FAIL;
	
	if (string_eq(command->command, "fade")) {
		if (string_eq(command->args[0], "start")) {
			start_fade();
		} else if (string_eq(command->args[0], "stop")) {
			stop_fade();
		} else if (string_eq(command->args[0], "set")) {
			if (command->args_count < 2) return SHELL_RESULT_FAIL;
			set_brightness(atoi(command->args[1]));
		} else {
			return SHELL_RESULT_FAIL;
		}
	}
	return SHELL_RESULT_SUCCESS;
}
