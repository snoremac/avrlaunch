
AVR_PREFIX = /usr/local/avr
AVR_CONF = $(AVR_PREFIX)/etc/avrdude.conf 
SHELL = /bin/sh

.SUFFIXES:
.SUFFIXES: .c .o

ARTIFACTS = *.o *.a *.elf *.hex

CC = avr-gcc
AR = avr-ar
OBJCOPY = avr-objcopy

MCU = atmega328p
FREQ = 16000000L

LOG_LEVEL ?= 2
UART_BAUD ?= 230400

DEFAULT_CFLAGS = \
	-std=gnu99 \
	-Os \
	-ffunction-sections -fdata-sections \
	-fshort-enums \
	-Wall \
	-mmcu=$(MCU) \
	-DF_CPU=$(FREQ) \
	-DUART_BAUD=$(UART_BAUD) \
	-DLOG_LEVEL=$(LOG_LEVEL)

DEBUG_CFLAGS = -g -gdwarf-2 -g3

DEFAULT_LDFLAGS = \
	-Os \
	-mmcu=$(MCU) \
	-lm \
	-Wl,--gc-sections
