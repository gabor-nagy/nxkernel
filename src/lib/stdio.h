#ifndef LIB_STDIO_H
#define LIB_STDIO_H

#include <stdarg.h>
#include "../arch/arch.h"

int getchar();

void printf(const char *format, ...);

#endif
