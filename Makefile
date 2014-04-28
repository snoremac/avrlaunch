export AVRLAUNCH_HOME = .
export AVRLAUNCH_SRC = $(AVRLAUNCH_HOME)/avrlaunch
export AVRLAUNCH_BUILD = $(AVRLAUNCH_HOME)/build
export UNITY_SRC = $(AVRLAUNCH_HOME)/unity
export ARDUINO_HOME = $(HOME)/software/arduino

.PHONY: all debug
all : avrlaunch test examples
debug : avrlaunch-debug

include mk/defs.mk
include mk/common.mk

override INCLUDES += -I. -I$(AVRLAUNCH_HOME) -I$(AVRLAUNCH_BUILD)
override INCLUDES += -I$(UNITY_SRC) -I$(AVR_PREFIX)/include -I/usr/local/include

avrlaunch_src = $(wildcard $(AVRLAUNCH_SRC)/*.c) \
	$(wildcard $(AVRLAUNCH_SRC)/buffer/*.c) \
	$(wildcard $(AVRLAUNCH_SRC)/hal/$(MCU)/*.c) \
	$(wildcard $(AVRLAUNCH_SRC)/event/*.c)

avrlaunch_obj = $(AVRLAUNCH_BUILD)/avrlaunch/pgmspace/pgm_strings.o $(avrlaunch_src:.c=.o)

# Empty SECONDARY stops intermediary files (e.g. %.elf, pgm_strings.c)
# from being deleted by make
.SECONDARY:

.PHONY: avrlaunch
avrlaunch : $(AVRLAUNCH_SRC)/avrlaunch.a

$(AVRLAUNCH_SRC)/avrlaunch.a : $(avrlaunch_obj)
	$(AR) rcs $@ $^ 

$(AVRLAUNCH_BUILD)/avrlaunch/pgmspace/pgm_strings.o : $(AVRLAUNCH_BUILD)/avrlaunch/pgmspace/pgm_strings.h

$(AVRLAUNCH_BUILD)/avrlaunch/pgmspace/pgm_strings.% : avrlaunch/pgmspace/pgm_strings.%.erb
	mkdir -p $(AVRLAUNCH_BUILD)/avrlaunch/pgmspace
	erb -r yaml $< > $@

.PHONY: program-bootloader
program-bootloader:
	make -C $(ARDUINO_HOME)/hardware/arduino/bootloaders/optiboot ISPTOOL=usbasp atmega328_isp

include mk/unity.mk
include mk/test.mk
include mk/examples.mk

.PHONY: clean
clean: test-clean examples-clean
	rm -rf $(AVRLAUNCH_BUILD)
	rm -f $(addprefix $(AVRLAUNCH_HOME)/, $(ARTIFACTS))
	rm -f $(addprefix $(AVRLAUNCH_SRC)/, $(ARTIFACTS))
	rm -f $(AVRLAUNCH_SRC)/buffer/*.o $(AVRLAUNCH_SRC)/hal/*.o
	rm -f $(AVRLAUNCH_SRC)/event/*.o $(AVRLAUNCH_SRC)/hal/$(MCU)/*.o
