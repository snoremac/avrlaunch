#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/hal/hal_uart.h"
#include "avrlaunch/buffer/buffer.h"
#include "avrlaunch/buffer/buffer_int.h"

#define UART_BUFFER_SIZE 8

static struct buffer uart_buffer;
static uint8_t uart_buffer_data[UART_BUFFER_SIZE];

static void set_baud(uint32_t baud);
static void configure_uart(void);

static void baud_9600(void);
static void baud_38400(void);
static void baud_38400(void);
static void baud_57600(void);
static void baud_115200(void);
static void baud_230400(void);

void uart_enable(uint32_t baud) {
	uart_buffer = buffer_init(uart_buffer_data, UART_BUFFER_SIZE, sizeof(uint8_t));
	set_baud(baud);
	configure_uart();
}

static void set_baud(uint32_t baud) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		switch (baud) {
			case 9600:
				baud_9600();
				break;
			case 38400:
				baud_38400();
				break;
			case 57600:
				baud_57600();
				break;
			case 115200:
				baud_115200();
				break;
			case 230400:
				baud_230400();
				break;
			default:
				baud_38400();
				break;
		}
	}
}

static void configure_uart(void) {
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		// 8 bit characters
		set_bit(&UCSR0C, UCSZ00);
		set_bit(&UCSR0C, UCSZ01);
		clear_bit(&UCSR0B, UCSZ02);

		// Asynchronous mode
		clear_bit(&UCSR0C, UMSEL00);
		clear_bit(&UCSR0C, UMSEL01);

		// No parity
		clear_bit(&UCSR0C, UPM00);
		clear_bit(&UCSR0C, UPM01);

		// Even parity
		// clear_bit(&UCSR0C, UPM00);
		// set_bit(&UCSR0C, UPM01);

		// 1 stop bit
		clear_bit(&UCSR0C, USBS0);

		// 2 stop bits
		// set_bit(&UCSR0C, USBS0);

		// Clock polarity (N/A in asynchronous mode)
		clear_bit(&UCSR0C, UCPOL0);

		// Enable RX
	  set_bit(&UCSR0B, RXEN0);
		
		// Enable receive complete interrupt
		set_bit(&UCSR0B, RXCIE0);

		// Enable TX
		set_bit(&UCSR0B, TXEN0);			
	}
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

void uart_putc(char c) {
	if (c == '\n') {
		uart_putc('\r');
	}
	uart_putc_raw(c);
}

void uart_putc_raw(char c) {
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c;
}

struct buffer* get_uart_buffer(void) {
	return &uart_buffer;
}

ISR(USART_RX_vect) {
	char c = UDR0;
	if (c == '\r') {
		c = '\n';
	}
	buffer_push_overflow(&uart_buffer, &c);
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

static void baud_57600() {
	#ifdef BAUD
	#undef BAUD
	#endif
	#define BAUD 57600
	#include <util/setbaud.h>
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	#if USE_2X
	UCSR0A |= (1 << U2X0);
	#else
	UCSR0A &= ~(1 << U2X0);
	#endif
}

static void baud_115200() {
	// Hard coded for F_CPU = 16000000 as <util/setbaud.h> doesn't
	// like the error margin
	UBRR0H = 0x00;
	UBRR0L = 0x10;
	UCSR0A |= (1 << U2X0);
}

static void baud_230400() {
	// Hard coded for F_CPU = 16000000 as <util/setbaud.h> doesn't
	// like the error margin
	UBRR0H = 0x00;
	UBRR0L = 0x08;
	UCSR0A |= (1 << U2X0);
}
