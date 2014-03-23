#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "sim_avr.h"
#include "sim_elf.h"
#include "uart_pty.h"

static uart_pty_t uart_pty;

static avr_t * load_firmware(const char *path);
static void run(avr_t *avr);
static void * avr_run_thread(void * param);
static void avr_special_deinit(avr_t* avr);

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Usage: %s FIRMWARE\n", argv[0]);
		exit(1);
	}


	avr_t * avr = load_firmware(argv[1]);

	uart_pty_init(avr, &uart_pty);
	uart_pty_connect(&uart_pty, '0');	

	printf("Firmware loaded, press the any key to run...\n");
	getchar();

	run(avr);
}

static avr_t * load_firmware(const char *path) {
	elf_firmware_t firmware;
	elf_read_firmware(path, &firmware);
	printf("Firmware: %s (mmcu = %s, freq = %d)\n", path, firmware.mmcu, (int) firmware.frequency);

	avr_t *avr = avr_make_mcu_by_name(firmware.mmcu);
	if (!avr) {
		fprintf(stderr, "AVR '%s' not known\n", firmware.mmcu);
		exit(1);
	}

	avr->special_deinit = avr_special_deinit;
	avr_init(avr);	
	avr_load_firmware(avr, &firmware);
	return avr;
}

static void run(avr_t * avr) {
	pthread_t run_thread;
	pthread_create(&run_thread, NULL, avr_run_thread, avr);
	while (1) {}
}

static void * avr_run_thread(void * param) {
	avr_t *avr = (avr_t *) param;
	while (1) {
		avr_run(avr);
	}
}

static void avr_special_deinit(avr_t* avr) {
	uart_pty_stop(&uart_pty);
}

