
.PHONY: examples blink fade
examples : blink fade
blink : blink.hex
fade : fade.hex

.PHONY: blink.hex fade.hex
blink.hex fade.hex : CFLAGS += -DAVRLAUNCH_OVERRIDE_DEFS
blink.hex fade.hex : INCLUDES += -I$(AVRLAUNCH_HOME)/examples
blink.hex : $(AVRLAUNCH_HOME)/examples/blink/blink.hex
fade.hex : $(AVRLAUNCH_HOME)/examples/fade/fade.hex

$(AVRLAUNCH_HOME)/examples/%.elf : $(AVRLAUNCH_HOME)/examples/%.o $(AVRLAUNCH_SRC)/avrlaunch.a $(LUFA_SRC)/libvirtual_serial.a
	# Position of LDFLAGS matters for correct linking of libm
	$(CC) -o $@ $^ $(DEFAULT_LDFLAGS) $(LDFLAGS)
	ln -sf $@ firmware.elf

.PHONY: examples-clean
examples-clean:
	rm -f $(addprefix $(AVRLAUNCH_HOME)/examples/blink/, $(ARTIFACTS))
	rm -f $(addprefix $(AVRLAUNCH_HOME)/examples/fade/, $(ARTIFACTS))
