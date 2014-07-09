
#ifndef LUFA_DESCRIPTORS_H
#define LUFA_DESCRIPTORS_H

#include <avr/pgmspace.h>
#include <LUFA/Drivers/USB/USB.h>

#define CDC_NOTIFICATION_EPADDR        (ENDPOINT_DIR_IN  | 2)
#define CDC_TX_EPADDR                  (ENDPOINT_DIR_IN  | 3)
#define CDC_RX_EPADDR                  (ENDPOINT_DIR_OUT | 4)
#define CDC_NOTIFICATION_EPSIZE        8
#define CDC_TXRX_EPSIZE                16

typedef struct {
	USB_Descriptor_Configuration_Header_t    Config;

	USB_Descriptor_Interface_t               CDC_CCI_Interface;
	USB_CDC_Descriptor_FunctionalHeader_t    CDC_Functional_Header;
	USB_CDC_Descriptor_FunctionalACM_t       CDC_Functional_ACM;
	USB_CDC_Descriptor_FunctionalUnion_t     CDC_Functional_Union;
	USB_Descriptor_Endpoint_t                CDC_NotificationEndpoint;

	USB_Descriptor_Interface_t               CDC_DCI_Interface;
	USB_Descriptor_Endpoint_t                CDC_DataOutEndpoint;
	USB_Descriptor_Endpoint_t                CDC_DataInEndpoint;
} USB_Descriptor_Configuration_t;

enum InterfaceDescriptors_t {
	INTERFACE_ID_CDC_CCI = 0,
	INTERFACE_ID_CDC_DCI = 1
};

enum StringDescriptors_t
{
	STRING_ID_Language     = 0,
	STRING_ID_Manufacturer = 1,
	STRING_ID_Product      = 2
};

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint8_t wIndex,
                                    const void** const DescriptorAddress)
                                    ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);

#endif

