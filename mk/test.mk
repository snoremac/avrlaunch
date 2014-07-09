TEST ?= BUFFER,SCHEDULER,SHELL,EVENT,ADC_EVENT,GPIO_EVENT,

test_src = $(wildcard $(AVRLAUNCH_SRC)/test/*.c)
test_obj = $(test_src:.c=.o)

comma := ,
modules = $(subst $(comma), ,$(TEST))

.PHONY: test
test : test.hex

.PHONY: test.hex
test.hex : $(AVRLAUNCH_SRC)/test/test.hex

$(AVRLAUNCH_SRC)/test/test.elf : CFLAGS += $(addprefix -DTEST_, $(modules)) -DAVRLAUNCH_OVERRIDE_DEFS
$(AVRLAUNCH_SRC)/test/test.elf : INCLUDES += -I$(AVRLAUNCH_SRC)/test
$(AVRLAUNCH_SRC)/test/test.elf : $(avrlaunch_obj) $(unity_obj) $(test_obj) $(AVRLAUNCH_SRC)/avrlaunch.a $(LUFA_SRC)/libvirtual_serial.a
	# Position of LDFLAGS matters for correct linking of libm
	$(CC) -o $@ $^ $(DEFAULT_LDFLAGS) $(LDFLAGS)
	ln -sf $@ firmware.elf  

.PHONY: test-clean
test-clean:
	rm -f $(addprefix $(AVRLAUNCH_SRC)/test/, $(ARTIFACTS))
	rm -f $(addprefix $(UNITY_SRC)/, $(ARTIFACTS))
