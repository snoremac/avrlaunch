#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/hal/hal_wave.h"

static volatile amplitude_callback next_amplitude;

void wave_timer_init() {
  // Timer 2
  // Prescale 1
  set_bit(&TCCR2B, CS20);
  clear_bit(&TCCR2B, CS21);
  clear_bit(&TCCR2B, CS22);

  // Phase Correct PWM, TOP at 0xFF
  set_bit(&TCCR2A, WGM20);
  clear_bit(&TCCR2A, WGM21);
  clear_bit(&TCCR2B, WGM22);

	// Set B3 as output
	set_bit(&DDRB, PIN3);
}

void wave_timer_on(amplitude_callback callback) {
	next_amplitude = callback;
	
	// Timer 2
	// Set output compare A to zero
	OCR2A = 0;

	// Connect output compare A (nont-inverting)
	set_bit(&TCCR2A, COM2A1);

  // Enable interrupt
  set_bit(&TIMSK2, TOIE2);
}

void wave_timer_off() {
  // Timer 2
	// Disconnect output compare A
	clear_bit(&TCCR2A, COM2A0);
	clear_bit(&TCCR2A, COM2A1);

  // Disable interrupt
  clear_bit(&TIMSK2, TOIE2);	
}

ISR(TIMER2_OVF_vect) {
	OCR2A = next_amplitude();
}
