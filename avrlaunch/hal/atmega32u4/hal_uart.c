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
		set_bit(&UCSR1C, UCSZ10);
		set_bit(&UCSR1C, UCSZ11);
		clear_bit(&UCSR1B, UCSZ12);

		// Asynchronous mode
		clear_bit(&UCSR1C, UMSEL10);
		clear_bit(&UCSR1C, UMSEL11);

		// No parity
		clear_bit(&UCSR1C, UPM10);
		clear_bit(&UCSR1C, UPM11);

		// Even parity
		// clear_bit(&UCSR1C, UPM10);
		// set_bit(&UCSR1C, UPM11);

		// 1 stop bit
		clear_bit(&UCSR1C, USBS1);

		// 2 stop bits
		// set_bit(&UCSR1C, USBS1);

		// Clock polarity (N/A in asynchronous mode)
		clear_bit(&UCSR1C, UCPOL1);

		// Enable RX
	  set_bit(&UCSR1B, RXEN1);
		
		// Enable receive complete interrupt
		set_bit(&UCSR1B, RXCIE1);

		// Enable TX
		set_bit(&UCSR1B, TXEN1);			
	}
}

void uart_disable() {
	uint8_t oldSREG = SREG;
	cli();

	// Disable TX / RX
	clear_bit(&UCSR1B, TXEN1);
	clear_bit(&UCSR1B, RXEN1);

	// Disable receive complete interrupt
	clear_bit(&UCSR1B, RXCIE1);

	SREG = oldSREG;
}

int uart_fputc(FILE* stream, char c) {
	uart_putc(c);
	return 0;
}

void uart_putc(char c) {
	loop_until_bit_is_set(UCSR1A, UDRE1);
	UDR1 = c;
}

struct buffer* uart_get_buffer(void) {
	return &uart_buffer;
}

ISR(USART1_RX_vect) {
	char c = UDR1;
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
	UBRR1H = UBRRH_VALUE;
	UBRR1L = UBRRL_VALUE;
	#if USE_2X
	UCSR1A |= (1 << U2X1);
	#else
	UCSR1A &= ~(1 << U2X1);
	#endif
}

static void baud_38400() {
	#ifdef BAUD
	#undef BAUD
	#endif
	#define BAUD 38400
	#include <util/setbaud.h>
	UBRR1H = UBRRH_VALUE;
	UBRR1L = UBRRL_VALUE;
	#if USE_2X
	UCSR1A |= (1 << U2X1);
	#else
	UCSR1A &= ~(1 << U2X1);
	#endif
}

static void baud_57600() {
	#ifdef BAUD
	#undef BAUD
	#endif
	#define BAUD 57600
	#include <util/setbaud.h>
	UBRR1H = UBRRH_VALUE;
	UBRR1L = UBRRL_VALUE;
	#if USE_2X
	UCSR1A |= (1 << U2X1);
	#else
	UCSR1A &= ~(1 << U2X1);
	#endif
}

static void baud_115200() {
	// Hard coded for F_CPU = 16000000 as <util/setbaud.h> doesn't
	// like the error margin
	UBRR1H = 0x00;
	UBRR1L = 0x10;
	UCSR1A |= (1 << U2X1);
}

static void baud_230400() {
	// Hard coded for F_CPU = 16000000 as <util/setbaud.h> doesn't
	// like the error margin
	UBRR1H = 0x00;
	UBRR1L = 0x08;
	UCSR1A |= (1 << U2X1);
}
