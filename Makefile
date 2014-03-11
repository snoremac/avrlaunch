export AVRLAUNCH_HOME = .
export AVRLAUNCH_SRC = $(AVRLAUNCH_HOME)/avrlaunch
export AVRLAUNCH_BUILD = $(AVRLAUNCH_HOME)/build
export UNITY_SRC = $(AVRLAUNCH_HOME)/unity

.PHONY: all debug
all : avrlaunch test examples
debug : avrlaunch-debug

include mk/defs.mk
include mk/common.mk

override INCLUDES += -I. -I$(AVRLAUNCH_HOME) -I$(AVRLAUNCH_BUILD)
override INCLUDES += -I$(UNITY_SRC) -I$(AVR_PREFIX)/include

avrlaunch_src = $(wildcard $(AVRLAUNCH_SRC)/*.c) $(wildcard $(AVRLAUNCH_SRC)/hal/$(MCU)/*.c) $(wildcard $(AVRLAUNCH_SRC)/event/*.c)
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

include mk/unity.mk
include mk/test.mk
include mk/examples.mk

.PHONY: clean
clean: test-clean examples-clean
	rm -rf $(AVRLAUNCH_BUILD)
	rm -f $(addprefix $(AVRLAUNCH_HOME)/, $(ARTIFACTS))
	rm -f $(addprefix $(AVRLAUNCH_SRC)/, $(ARTIFACTS))
	rm -f $(AVRLAUNCH_SRC)/hal/*.o $(AVRLAUNCH_SRC)/event/*.o $(AVRLAUNCH_SRC)/hal/$(MCU)/*.o $(AVRLAUNCH_SRC)/hal/mock/*.o


