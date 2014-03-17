#ifndef AVRLAUNCH_WAVE_H
#define AVRLAUNCH_WAVE_H

typedef enum waveform_type {
	WAVE_SAWTOOTH, WAVE_SINE, WAVE_SQUARE, WAVE_TRIANGLE
} waveform_type;

void wave_init(void);

void wave_on(waveform_type waveform, float hertz);

void wave_off(void);

#endif
