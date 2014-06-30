#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/hal/hal_wave.h"

static volatile amplitude_callback next_amplitude;  

void wave_timer_init() {
  // Timer 3
  // Prescale 1
  set_bit(&TCCR3B, CS30);
  clear_bit(&TCCR3B, CS31);
  clear_bit(&TCCR3B, CS32);

  // Phase Correct PWM, TOP at 0xFF
  set_bit(&TCCR3A, WGM30);
  clear_bit(&TCCR3A, WGM31);
  clear_bit(&TCCR3B, WGM32);
  clear_bit(&TCCR3B, WGM33);

	// Set pin C6 as output
	set_bit(&DDRC, PIN6);
}

void wave_timer_on(amplitude_callback callback) {
	next_amplitude = callback;
	
	// Timer 3
	// Set output compare A to zero
	OCR3A = 0;

	// Connect output compare A (clear on up-counting)
	clear_bit(&TCCR3A, COM3A0);
	set_bit(&TCCR3A, COM3A1);

  // Enable interrupt
  set_bit(&TIMSK2, TOIE2);
}

void wave_timer_off() {
  // Timer 2
	// Disconnect output compare A
	clear_bit(&TCCR2A, COM2A0);
	clear_bit(&TCCR2A, COM2A1);

  // Disable interrupt
  clear_bit(&TIMSK3, TOIE3);
}

ISR(TIMER3_OVF_vect) {
	OCR3A = next_amplitude();
}
