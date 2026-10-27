#ifndef UART_H
#define UART_H

#include <stdbool.h>

// The badge has no hardware UART; this is serial over the USB-C cable
// (shows up as /dev/ttyACM0 on Linux, a COM port on Windows).

void uart_init_serial(void);

// Non-blocking. Collects characters; when a full line arrives (Enter),
// copies it into out (null-terminated, without the newline) and returns true.
bool uart_read_line(char *out, int out_size);

#endif
