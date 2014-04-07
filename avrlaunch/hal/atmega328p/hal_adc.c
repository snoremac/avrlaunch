
#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/log.h"
#include "avrlaunch/shell.h"
#include "avrlaunch/pgmspace/pgm_strings.h"
#include "avrlaunch/hal/atmega328p/hal_adc.h"

#define NO_DUMMY -1

static int8_t dummy_channel = NO_DUMMY;

static void error_not_adc_channel(const uint8_t channel);
static shell_result adc_shell_handler(shell_command* command);

void adc_init(void) {
  ADMUX &= 0x00;
  ADCSRA &= 0x00;
  
	// Set ADC prescale factor to 128
	set_bit(&ADCSRA, ADPS2);
	set_bit(&ADCSRA, ADPS1);
	set_bit(&ADCSRA, ADPS0);

  // Disable power reduction
	clear_bit(&PRR, PRADC);

	// Enable the ADC
	set_bit(&ADCSRA, ADEN);
	
	// Set reference voltage to AVcc
	set_bit(&ADMUX, REFS0);

	// Set to full 10 bit operation
	clear_bit(&ADMUX, ADLAR);

  PGM_STR(ADC_SHELL_ADC, shell_adc)
  shell_register_handler(shell_adc, adc_shell_handler);
}

void adc_enable_dummy(uint8_t channel) {
  if (channel > 5) {
    error_not_adc_channel(channel);
    return;
  }
  dummy_channel = channel;
}

uint16_t adc_read(uint8_t channel) {
  if (channel > 5) {
    error_not_adc_channel(channel);
    return 0;
  }

  if (dummy_channel != NO_DUMMY) {
    // Take a reading from the dummy channel to try and clear the S&H circuitry
  	ADMUX = (ADMUX & 0xf0) | dummy_channel;	
  	set_bit(&ADCSRA, ADSC);
  	while (bit_is_set(ADCSRA, ADSC));
  }

	// Set ADC input channel
	ADMUX = (ADMUX & 0xf0) | channel;	

	// Start the conversion
	set_bit(&ADCSRA, ADSC);
	while (bit_is_set(ADCSRA, ADSC));
	
	// Read the low byte first!
	uint8_t low = ADCL;
	uint8_t high = ADCH;

	return (high << 8) | low;
}

static void error_not_adc_channel(const uint8_t channel) {
  set_system_error();
	if (LOG_LEVEL >= LOG_LEVEL_ERROR) {
    PGM_STR(ADC_NO_CHANNEL, failed_msg)
  	LOG_ERROR(failed_msg, channel);
	}		
}

static shell_result adc_shell_handler(shell_command* command) {
	if (!string_eq(command->command, "adc")) return SHELL_RESULT_FAIL;
	if (command->args_count == 0) return SHELL_RESULT_FAIL;
	
  if (string_eq(command->args[0], "read")) {
    shell_printf("Channel\tReading\n");
  	if (command->args_count == 2) {
  		uint8_t channel = atoi(command->args[1]);
    	shell_printf("%u\t%u\n", channel, adc_read(channel));
  	} else {
      for (int i = 0; i < 6; i++) {
      	shell_printf("%u\t%u\n", i, adc_read(i));
      }
  	}
  	return SHELL_RESULT_SUCCESS;
	}
	return SHELL_RESULT_FAIL;
}
