#include <math.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/shell.h"
#include "avrlaunch/wave.h"
#include "avrlaunch/hal/hal_gpio.h"

prog_uchar sine256[] PROGMEM = {
  127,130,133,136,139,143,146,149,152,155,158,161,164,167,170,173,
  176,178,181,184,187,190,192,195,198,200,203,205,208,210,212,215,
  217,219,221,223,225,227,229,231,233,234,236,238,239,240,242,243,
  244,245,247,248,249,249,250,251,252,252,253,253,253,254,254,254,
  254,254,254,254,253,253,253,252,252,251,250,249,249,248,247,245,
  244,243,242,240,239,238,236,234,233,231,229,227,225,223,221,219,
  217,215,212,210,208,205,203,200,198,195,192,190,187,184,181,178,
  176,173,170,167,164,161,158,155,152,149,146,143,139,136,133,130,
  127,124,121,118,115,111,108,105,102,99,96,93,90,87,84,81,78,76,
  73,70,67,64,62,59,56,54,51,49,46,44,42,39,37,35,33,31,29,27,25,
  23,21,20,18,16,15,14,12,11,10,9,7,6,5,5,4,3,2,2,1,1,1,0,0,0,0,
  0,0,0,1,1,1,2,2,3,4,5,5,6,7,9,10,11,12,14,15,16,18,20,21,23,25,
  27,29,31,33,35,37,39,42,44,46,49,51,54,56,59,62,64,67,70,73,76,
  78,81,84,87,90,93,96,99,102,105,108,111,115,118,121,124
};

typedef uint8_t (*amplitude_converter)(uint8_t phase_position);

static gpio wave_gpio;
static gpio_timer wave_gpio_timer;

static volatile long phase_accumulator;
static volatile long tuning_word;

static amplitude_converter current_converter;

static uint8_t sawtooth_converter(uint8_t phase_position);
static uint8_t sine_converter(uint8_t phase_position);
static uint8_t square_converter(uint8_t phase_position);
static uint8_t triangle_converter(uint8_t phase_position);

static void configure_timer(void);

void wave_init() {
  wave_gpio = GPIO(PORTB, PIN3);
  wave_gpio_timer = GPIO_TIMER(PORTB, PIN3);

  gpio_set_output(&wave_gpio);
  configure_timer();
  wave_off();
}

void wave_on(waveform_type waveform, float hertz) {
  cli();

  switch (waveform) {
    case WAVE_SAWTOOTH:
      current_converter = sawtooth_converter;
      break;
    case WAVE_SINE:
      current_converter = sine_converter;
      break;
    case WAVE_SQUARE:
      current_converter = square_converter;
      break;
    case WAVE_TRIANGLE:
      current_converter = triangle_converter;
      break;
  }

  phase_accumulator = 0;
  tuning_word = pow(2, 32) * hertz / (F_CPU / 510);

  gpio_set_output_compare(&wave_gpio_timer, 0);
  gpio_connect_timer_non_inverting(&wave_gpio_timer);

  // Enable interrupt
  set_bit(&TIMSK2, TOIE2);

  sei();
}

void wave_off() {
  gpio_disconnect_timer(&wave_gpio_timer);

  // Disable interrupt
  clear_bit(&TIMSK2, TOIE2);
}

ISR(TIMER2_OVF_vect) {
  uint8_t phase_position = (uint8_t) (phase_accumulator >> 24);
  gpio_set_output_compare(&wave_gpio_timer, current_converter(phase_position));
  phase_accumulator += tuning_word;
}

static uint8_t sawtooth_converter(uint8_t phase_position) {
  return phase_position;
}

static uint8_t sine_converter(uint8_t phase_position) {
  return pgm_read_byte_near(sine256 + phase_position);
}

static uint8_t square_converter(uint8_t phase_position) {
  return phase_position < 128 ? 255 : 0;
}

static uint8_t triangle_converter(uint8_t phase_position) {
  return phase_position < 128 ? phase_position * 2 : (128 - (phase_position - 127)) * 2;
}

static void configure_timer() {
  // Timer 2
  // Prescale 1
  set_bit(&TCCR2B, CS20);
  clear_bit(&TCCR2B, CS21);
  clear_bit(&TCCR2B, CS22);

  // Phase Correct PWM, TOP at 0xFF
  set_bit(&TCCR2A, WGM20);
  clear_bit(&TCCR2A, WGM21);
  clear_bit(&TCCR2B, WGM22);
}
