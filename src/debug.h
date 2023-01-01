#pragma once

#include <stddef.h>
#include <inttypes.h>

void debug_put(char c);
char debug_get(void);
void debug_printf(const char *format, ...);
void fatal(const char *format, ...);
void hexdump(const uint8_t *buf, size_t sz);
