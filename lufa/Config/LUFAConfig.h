#ifndef _LUFA_CONFIG_H_
#define _LUFA_CONFIG_H_

	#if (ARCH == ARCH_AVR8)
		#define USE_STATIC_OPTIONS               (USB_DEVICE_OPT_FULLSPEED | USB_OPT_REG_ENABLED | USB_OPT_AUTO_PLL)
		#define USB_DEVICE_ONLY
		#define USE_FLASH_DESCRIPTORS
		#define FIXED_CONTROL_ENDPOINT_SIZE      8
		#define FIXED_NUM_CONFIGURATIONS         1
		#define INTERRUPT_CONTROL_ENDPOINT
	#else
		#error Unsupported architecture for this LUFA configuration file.
	#endif
#endif