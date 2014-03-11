#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/io.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/hal/hal_uart.h"

static void baud_9600(void);
static void baud_38400(void);

void uart_enable(uint32_t baud) {
	uint8_t oldSREG = SREG;
	cli();

	switch (baud) {
		case 9600:
			baud_9600();
			break;
		default:
			baud_38400();
			break;
	}

	// Setup framing - 8N1
	UCSR0C = 0;
	set_bit(&UCSR0C, UCSZ01);
	set_bit(&UCSR0C, UCSZ00);

	// Enable RX
  set_bit(&UCSR0B, RXEN0);
	
	// Enable receive complete interrupt
	set_bit(&UCSR0B, RXCIE0);

	// Enable TX
	set_bit(&UCSR0B, TXEN0);

	SREG = oldSREG;
	sei();
}

void uart_disable() {
	uint8_t oldSREG = SREG;
	cli();

	// Disable TX / RX
	clear_bit(&UCSR0B, TXEN0);
	clear_bit(&UCSR0B, RXEN0);

	// Disable receive complete interrupt
	clear_bit(&UCSR0B, RXCIE0);

	SREG = oldSREG;
}

char uart_pollc() {
	loop_until_bit_is_set(UCSR0A, RXC0);

	if (bit_is_set(UCSR0A, FE0)) {
		return _FDEV_EOF;
	}
	if (bit_is_set(UCSR0A, DOR0)) {
		return _FDEV_ERR;
	}
	char c = UDR0;
	if (c == '\r') {
		c = '\n';
	}
	return c;
}

void uart_putc(char c) {
	if (c == '\n') {
		uart_putc('\r');
	}
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c;
}

static void baud_9600() {
	#ifdef BAUD
	#undef BAUD
	#endif
	#define BAUD 9600
	#include <util/setbaud.h>
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	#if USE_2X
	UCSR0A |= (1 << U2X0);
	#else
	UCSR0A &= ~(1 << U2X0);
	#endif
}

static void baud_38400() {
	#ifdef BAUD
	#undef BAUD
	#endif
	#define BAUD 38400
	#include <util/setbaud.h>
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	#if USE_2X
	UCSR0A |= (1 << U2X0);
	#else
	UCSR0A &= ~(1 << U2X0);
	#endif
}
