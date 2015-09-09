#ifndef IA32_LOWLEVEL_H
#define IA32_LOWLEVEL_H

#include "types.h"

#define _in(port, data)     __asm__("in %%dx, %%al" : "=a" (data) : "d" (port));
#define _out(port, data)    __asm__("out %%al, %%dx" : : "a" (data), "d" (port));

inline u8 in(u16 port);                  /* read from port */
inline u8 out(u16 port, u8 data);        /* write to port, return data written */

inline void memcpy(void *dest, void *source, u32 count);                   /* copy count bytes of memory */
inline void memset(void *buffer, u32 value, u32 count, u8 value_size);    /* fill memory block with value */

#endif
