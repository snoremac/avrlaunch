#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "avr_uart.h"
#include "sim_irq.h"


#include "sim_avr.h"
#include "sim_elf.h"

enum {
	IRQ_UART_PTY_BYTE_IN,
	IRQ_UART_PTY_BYTE_OUT,
	IRQ_UART_PTY_COUNT
};

static const char * irq_names[IRQ_UART_PTY_COUNT] = {
	[IRQ_UART_PTY_BYTE_IN] = "8<uart_pty.in",
	[IRQ_UART_PTY_BYTE_OUT] = "8>uart_pty.out",
};

static avr_t * load_firmware(const char *path);
static void uart_init(avr_t* avr, char uart);
static void run(avr_t *avr);
static void * avr_run_thread(void * param);
static void uart_on_byte_in(struct avr_irq_t * irq, uint32_t value, void * param);

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Usage: %s FIRMWARE\n", argv[0]);
		exit(1);
	}

	avr_t * avr = load_firmware(argv[1]);
	uart_init(avr, '0');
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

	avr_init(avr);	
	avr_load_firmware(avr, &firmware);
	return avr;
}

static void uart_init(avr_t* avr, char uart) {
	avr_irq_t* irq = avr_alloc_irq(&avr->irq_pool, 0, IRQ_UART_PTY_COUNT, irq_names);
	avr_irq_register_notify(irq + IRQ_UART_PTY_BYTE_IN, uart_on_byte_in, NULL);
	avr_irq_t * src = avr_io_getirq(avr, AVR_IOCTL_UART_GETIRQ(uart), UART_IRQ_OUTPUT);
	avr_irq_t * dst = avr_io_getirq(avr, AVR_IOCTL_UART_GETIRQ(uart), UART_IRQ_INPUT);
	if (src && dst) {
		avr_connect_irq(src, irq + IRQ_UART_PTY_BYTE_IN);
		avr_connect_irq(irq + IRQ_UART_PTY_BYTE_OUT, dst);
	}
}

static void run(avr_t * avr) {
	pthread_t run_thread;
	pthread_create(&run_thread, NULL, avr_run_thread, avr);
	pthread_join(run_thread, NULL);
}

static void * avr_run_thread(void * param) {
	avr_t *avr = (avr_t *) param;

	while (1) {
		int state = avr_run(avr);
		if (state == cpu_Sleeping) {
			break;			
		}
	}
	printf("ended\n");
	return NULL;
}

static void uart_on_byte_in(struct avr_irq_t * irq, uint32_t value, void * param) {
	putchar(value);
}


