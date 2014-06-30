#ifndef AVRLAUNCH_HAL_H
#define AVRLAUNCH_HAL_H

#define XSTR(S) STR(S)
#define STR(S) #S

#if defined (__AVR_ATmega328P__) || defined (__AVR_ATmega328__)
	#define HAL_MODULE(NAME) XSTR(avrlaunch/hal/atmega328p/hal_ ## NAME.h)
#elif defined (__AVR_ATmega32U4__)
	#define HAL_MODULE(NAME) XSTR(avrlaunch/hal/atmega32u4/hal_ ## NAME.h)
#endif

#endif
