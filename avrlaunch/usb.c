
#include <inttypes.h>

#include "lufa/virtual_serial.h"

#include "avrlaunch/avrlaunch.h"
#include "avrlaunch/gpio.h"
#include "avrlaunch/scheduler.h"
#include "avrlaunch/usb.h"

#define USB_BUFFER_SIZE 8

static FILE usb_output_stream;
static struct buffer usb_buffer;
static uint8_t usb_buffer_data[USB_BUFFER_SIZE];

static void usb_task(struct task* task);
static void on_byte_received(char byte);

void usb_init() {
	usb_buffer = buffer_init(usb_buffer_data, USB_BUFFER_SIZE, sizeof(uint8_t));
	scheduler_add_task(&(struct task_config){"usb", TASK_FOREVER, TASK_ASAP}, usb_task, NULL);
	virtual_serial_init(&usb_output_stream, on_byte_received);
}

FILE* usb_get_output_stream() {
	return &usb_output_stream;
}

struct buffer* usb_get_buffer() {
	return &usb_buffer;
}

static void usb_task(struct task* task) {
	virtual_serial_task();
}

static void on_byte_received(char byte) {
	if (byte == '\r') {
		byte = '\n';
	}
	buffer_push_overflow(&usb_buffer, &byte);
}
