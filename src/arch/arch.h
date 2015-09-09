#ifndef ARCH_H
#define ARCH_H

/* include architecture-dependent code */

#if (ARCH == IA32)
   #include "ia32/arch.h"
#else
   #include "Unknown architecture. Maybe incorrect 'sys/config.h' ARCH setting."
#endif

#endif
