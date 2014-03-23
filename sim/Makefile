SIM_HOME = .
AVR_PREFIX = /usr/local/avr
SIMAVR_PREFIX = /usr/local/src/avr/simavr

DARWIN_ARCH = $(shell uname -a |awk '{ print $$15 }')
DARWIN_RELEASE = $(shell uname -a |awk '{ print $$3 }')
SIMAVR_OBJ_PATH = $(SIMAVR_PREFIX)/examples/board_simduino/obj-$(DARWIN_ARCH)-apple-darwin$(DARWIN_RELEASE)

CC = clang
INCLUDES = -I. -I$(AVR_PREFIX)/include -I$(SIMAVR_PREFIX)/examples/parts
VPATH = $(SIMAVR_PREFIX)/examples/board_simduino/obj-x86_64-apple-darwin12.5.0

DEFAULT_CFLAGS = -O2 -Wall --std=gnu99
DEBUG_CFLAGS = -g -gdwarf-2 -g3
DEFAULT_LDFLAGS = -L/opt/local/lib -L$(AVR_PREFIX)/lib -lelf -lpthread -lsimavr

.PHONY: all
all : sim-basic

%.o : %.c
	$(CC) -c $(INCLUDES) $(CPPFLAGS) $(DEFAULT_CFLAGS) $(CFLAGS) $< -o $@

sim-% : sim-%.o $(SIMAVR_OBJ_PATH)/uart_pty.o
	$(CC) $(DEFAULT_LDFLAGS) $(LDFLAGS) -o $@ $^

$(SIMAVR_OBJ_PATH)/uart_pty.o :
	make -C $(SIMAVR_PREFIX) build-examples

clean : 
	make -C $(SIMAVR_PREFIX) clean
	rm -f $(SIM_HOME)/*.o
	rm -f sim-basic
