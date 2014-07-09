
#include <LUFA/Drivers/Board/LEDs.h>
#include <LUFA/Drivers/Board/Joystick.h>
#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Platform/Platform.h>

#include "descriptors.h"
#include "virtual_serial.h"

USB_ClassInfo_CDC_Device_t cdc_interface = {
		.Config = {
				.ControlInterfaceNumber   = INTERFACE_ID_CDC_CCI,
				.DataINEndpoint           = {
						.Address          = CDC_TX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
				},
				.DataOUTEndpoint = {
						.Address          = CDC_RX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
				},
				.NotificationEndpoint = {
						.Address          = CDC_NOTIFICATION_EPADDR,
						.Size             = CDC_NOTIFICATION_EPSIZE,
						.Banks            = 1,
				}
		}
};

static byte_received_handler byte_handler;

void virtual_serial_init(FILE* output_stream, byte_received_handler handler) {
	USB_Init();
	CDC_Device_CreateStream(&cdc_interface, output_stream);
	byte_handler = handler;
}

void virtual_serial_task() {
	if (USB_DeviceState == DEVICE_STATE_Configured) {
		int16_t byte_received = CDC_Device_ReceiveByte(&cdc_interface);
		if (byte_received >= 0) {
			byte_handler((uint8_t) byte_received);
		}
	}

	CDC_Device_USBTask(&cdc_interface);
	USB_USBTask();
}

int virtual_serial_fputc(char c, FILE* stream) {
	CDC_Device_SendByte(&cdc_interface, c);
	return 0;	
}

void EVENT_USB_Device_ConfigurationChanged(void) {
	CDC_Device_ConfigureEndpoints(&cdc_interface);
}

void EVENT_USB_Device_ControlRequest(void) {
	CDC_Device_ProcessControlRequest(&cdc_interface);
}


