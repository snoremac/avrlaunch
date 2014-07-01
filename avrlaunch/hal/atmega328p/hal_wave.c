#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/hal/hal_wave.h"

static wave_input_capture_callback on_input_capture;
static wave_duty_cycle_callback next_duty_cycle;
static wave_edge current_capture_edge;

void wave_in_timer_init() {
	// Timer 1
	// Normal mode, TOP at 0xFFFF
	clear_bit(&TCCR1A, WGM10);
	clear_bit(&TCCR1A, WGM11);
	clear_bit(&TCCR1B, WGM12);
	clear_bit(&TCCR1B, WGM13);

	// Prescaler of 1024
	set_bit(&TCCR1B, CS10);
	clear_bit(&TCCR1B, CS11);
	set_bit(&TCCR1B, CS12);
	
	// Set pin B0 as output
	clear_bit(&DDRB, PIN0);
}

void wave_in_timer_on(wave_input_capture_callback callback, wave_edge capture_edge) {
  on_input_capture = callback;
  current_capture_edge = capture_edge;

  if (current_capture_edge == WAVE_EDGE_FALLING) {
  	// Input capture on falling edge
  	clear_bit(&TCCR1B, ICES1);    
  } else {
  	// Input capture on rising edge
  	set_bit(&TCCR1B, ICES1);        
  }

	// Enable input capture interrupt
	set_bit(&TIMSK1, ICIE1);  
}

void wave_in_timer_off() {
	// Disable input capture interrupt
	clear_bit(&TIMSK1, ICIE1);    
}

void wave_in_timer_reset() {
  if (current_capture_edge == WAVE_EDGE_FALLING) {
  	// Input capture on falling edge
  	clear_bit(&TCCR1B, ICES1);    
  } else {
  	// Input capture on rising edge
  	set_bit(&TCCR1B, ICES1);        
  }

	// Reset counter to zero
	TCNT1H = 0;
	TCNT1L = 0;		
}

uint16_t wave_in_current_tick() {
	uint8_t timer_low = TCNT1L;
	uint8_t timer_high = TCNT1H;
	return (timer_high << 8) | timer_low;
}

ISR(TIMER1_CAPT_vect) {
	uint8_t timer_low = TCNT1L;
	uint8_t timer_high = TCNT1H;
	
  if (current_capture_edge == WAVE_EDGE_BOTH) {
  	// Toggle input capture edge selection
  	toggle_bit(&TCCR1B, ICES1);

  	// Reset input capture interrupt flag
  	set_bit(&TIFR1, ICF1);
  }
	
  on_input_capture((timer_high << 8) | timer_low);

	// Reset counter to zero
	TCNT1H = 0;
	TCNT1L = 0;
}

void wave_out_timer_init() {
  // Timer 2
  // Prescale 1
  set_bit(&TCCR2B, CS20);
  clear_bit(&TCCR2B, CS21);
  clear_bit(&TCCR2B, CS22);

  // Phase Correct PWM, TOP at 0xFF
  set_bit(&TCCR2A, WGM20);
  clear_bit(&TCCR2A, WGM21);
  clear_bit(&TCCR2B, WGM22);

	// Set pin B3 as output
	set_bit(&DDRB, PIN3);
}

void wave_out_timer_on(wave_duty_cycle_callback callback) {
	next_duty_cycle = callback;
	
	// Timer 2
	// Set output compare A to zero
	OCR2A = 0;

	// Connect output compare A (clear on up-counting)
	clear_bit(&TCCR2A, COM2A0);
	set_bit(&TCCR2A, COM2A1);

  // Enable interrupt
  set_bit(&TIMSK2, TOIE2);
}

void wave_out_timer_off() {
  // Timer 2
	// Disconnect output compare A
	clear_bit(&TCCR2A, COM2A0);
	clear_bit(&TCCR2A, COM2A1);

  // Disable interrupt
  clear_bit(&TIMSK2, TOIE2);	
}

ISR(TIMER2_OVF_vect) {
	OCR2A = next_duty_cycle();
}
