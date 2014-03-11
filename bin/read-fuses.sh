#!/bin/sh

# From http://lists.gnu.org/archive/html/avr-gcc-list/2005-08/msg00007.html

avrdude -p atmega328p -P tty.mcu -c stk500 -U hfuse:r:high.txt:s -U lfuse:r:low.txt:s

# Although may be only possible with a hardware programmer?
# http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=106380
