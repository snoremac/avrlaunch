
#include <math.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/scheduler.h"
#include "avrlaunch/shell.h"
#include "avrlaunch/tone.h"
#include "avrlaunch/wave.h"

struct note_sequence {
	const struct note* notes;
	uint8_t length;
	uint8_t current_note;
};

static struct note_sequence current_sequence;
static uint8_t play_task_id;

static void play_task(struct task* task);
static void tone_off_task(struct task* task);

void tone_init() {
	wave_init();
}

void tone_note(struct note note) {
	if (note.pitch > 0) {
		wave_on(WAVE_SQUARE, note.pitch);
	}
	scheduler_add_task(&(struct task_config){"toff", TASK_ONCE, note.duration}, tone_off_task, NULL);
}

void tone_play(const struct note* notes, uint8_t notes_length) {
	wave_off();
	scheduler_remove_task(play_task_id);
	current_sequence = (struct note_sequence) { notes, notes_length, 0 };
	play_task_id = scheduler_add_task(&(struct task_config){"play", TASK_ONCE, TASK_ASAP}, play_task, &current_sequence);
}

static void play_task(struct task* task) {
	wave_off();
	struct note_sequence* sequence = (struct note_sequence*) task->data;
	
	const struct note* note = &sequence->notes[sequence->current_note];
	float pitch = pgm_read_float(&note->pitch);
	uint16_t duration = pgm_read_word(&note->duration);
	
	if (pitch > 0) {
		wave_on(WAVE_SQUARE, pitch);
	}
	
	sequence->current_note++;
	if (sequence->current_note < sequence->length) {
		scheduler_add_task(&(struct task_config){"play", TASK_ONCE, duration}, play_task, sequence);		
	} else {
		scheduler_add_task(&(struct task_config){"toff", TASK_ONCE, duration}, tone_off_task, NULL);
	}
}

static void tone_off_task(struct task* task) {
	wave_off();
}

