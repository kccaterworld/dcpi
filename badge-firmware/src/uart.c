#include "uart.h"
#include "pico/stdlib.h"
#include <string.h>

#define LINE_MAX 128

static char line[LINE_MAX];
static int line_len = 0;

void uart_init_serial(void) {
    stdio_init_all();
}

bool uart_read_line(char *out, int out_size) {
    while (true) {
        int c = getchar_timeout_us(0);
        if (c == PICO_ERROR_TIMEOUT) return false;

        if (c == '\r' || c == '\n') {
            if (line_len == 0) continue; // ignore empty lines / the \n after \r
            line[line_len] = '\0';
            strncpy(out, line, out_size - 1);
            out[out_size - 1] = '\0';
            line_len = 0;
            return true;
        }

        if (line_len < LINE_MAX - 1) {
            line[line_len++] = (char)c;
        }
        // Characters past LINE_MAX-1 are dropped.
    }
}
