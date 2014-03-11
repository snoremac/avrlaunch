
%.o : %.S
	$(CC) -c $(INCLUDES) $(CPPFLAGS) $(DEFAULT_CFLAGS) $(CFLAGS) $< -o $@

%.o : %.c
	$(CC) -c $(INCLUDES) $(CPPFLAGS) $(DEFAULT_CFLAGS) $(CFLAGS) $< -o $@

%.pre : %.c
	$(CC) -E -c $(INCLUDES) $(CPPFLAGS) $(DEFAULT_CFLAGS) $(CFLAGS) $< -o $@

%.pre : %.S
	$(CC) -E -c $(INCLUDES) $(CPPFLAGS) $(DEFAULT_CFLAGS) $(CFLAGS) $< -o $@

%.eep : %.elf
	$(OBJCOPY) -O ihex -j .eeprom \
		--set-section-flags=.eeprom=alloc,load --no-change-warnings \
		--change-section-lma .eeprom=0 \
		$< $@	 

%.hex : %.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@
	ln -sf $@ firmware.hex
	avr-size $<

.PHONY: %-debug
%-debug : CFLAGS += $(DEBUG_CFLAGS)
%-debug : %.hex
	$()

.PHONY: %-sim
%-sim : CFLAGS += -DSIM
%-sim : LDFLAGS += -Wl,--undefined=_mmcu,--section-start=.mmcu=0x910000
%-sim : %.hex
	$()

.PHONY: %-debug-sim
%-debug-sim : CFLAGS += $(DEBUG_CFLAGS) -DSIM
%-debug-sim : LDFLAGS += -Wl,--undefined=_mmcu,--section-start=.mmcu=0x910000
%-debug-sim : %.hex
	$()

.PHONY: program
program :
	$(AVRLAUNCH_HOME)/bin/kill_minicom.sh
	
	# Via USB + bootloader
	avrdude -F -D -C $(AVR_CONF) -p $(MCU) -P $(AVRLAUNCH_HOME)/tty.mcu -c stk500v1 -b 115200 -U flash:w:firmware.hex:i

	# Via AVR ISP mkII
	# avrdude -C $(AVR_CONF) -p $(MCU) -P usb:000200071799 -c avrispmkii -U flash:w:firmware.hex:i

.PHONY: program-sim
program-sim :
	simavr firmware.elf
