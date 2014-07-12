#include <stdio.h>
#include <avr/pgmspace.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/gpio.h"
#include "avrlaunch/log.h"
#include "avrlaunch/shell.h"
#include "avrlaunch/hal/hal_gpio.h"
#include "avrlaunch/pgmspace/pgm_strings.h"

void LOG(const char* level, const char* format, ...) {
  fprintf(shell_get_stream(), "[%s] ", level);
	va_list args;
	va_start(args, format);
	vfprintf(shell_get_stream(), format, args);
	va_end(args);
}

void set_system_warning() {
#if defined (__AVR_ATmega328P__) || defined (__AVR_ATmega328__)
  gpio_set_output(&GPIO(PORTB, PIN5));
  gpio_write_high(&GPIO(PORTB, PIN5));
#elif defined (__AVR_ATmega32U4__)
  gpio_set_output(&GPIO(PORTC, PIN7));
  gpio_write_high(&GPIO(PORTC, PIN7));
#endif	
}

void clear_system_warning() {
#if defined (__AVR_ATmega328P__) || defined (__AVR_ATmega328__)
  gpio_set_output(&GPIO(PORTB, PIN5));
  gpio_write_low(&GPIO(PORTB, PIN5));
#elif defined (__AVR_ATmega32U4__)
  gpio_set_output(&GPIO(PORTC, PIN7));
  gpio_write_low(&GPIO(PORTC, PIN7));
#endif	
}

void set_system_error() {
#if defined (__AVR_ATmega328P__) || defined (__AVR_ATmega328__)
  gpio_set_output(&GPIO(PORTB, PIN5));
  gpio_write_high(&GPIO(PORTB, PIN5));
#elif defined (__AVR_ATmega32U4__)
  gpio_set_output(&GPIO(PORTC, PIN7));
  gpio_write_high(&GPIO(PORTC, PIN7));
#endif	
}

void clear_system_error() {
#if defined (__AVR_ATmega328P__) || defined (__AVR_ATmega328__)
  gpio_set_output(&GPIO(PORTB, PIN5));
  gpio_write_low(&GPIO(PORTB, PIN5));
#elif defined (__AVR_ATmega32U4__)
  gpio_set_output(&GPIO(PORTC, PIN7));
  gpio_write_low(&GPIO(PORTC, PIN7));
#endif	
}

