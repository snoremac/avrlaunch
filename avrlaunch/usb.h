#ifndef AVRLAUNCH_USB_H
#define AVRLAUNCH_USB_H

#include <stdio.h>

#include "avrlaunch/buffer/buffer.h"

void usb_init(void);
FILE* usb_get_output_stream(void);
struct buffer* usb_get_buffer(void);

#endif
