#ifndef AVRLAUNCH_TONE_H
#define AVRLAUNCH_TONE_H

#include "avrlaunch/tone_freq.h"

struct note {
	float pitch;
	uint16_t duration;
};

void tone_init(void);

void tone_note(struct note note);

void tone_play(const struct note* notes, uint8_t notes_length);

#endif