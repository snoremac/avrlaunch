#ifndef LUFA_VIRTUAL_SERIAL_H
#define LUFA_VIRTUAL_SERIAL_H

#include <stdio.h>

typedef void (*byte_received_handler)(char byte);

void virtual_serial_init(FILE* output_stream, byte_received_handler handler);
void virtual_serial_task(void);
int virtual_serial_fputc(char c, FILE* stream);

#endif
