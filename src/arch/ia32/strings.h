#ifndef IA32_STRINGS_H
#define IA32_STRINGS_H

#include "lowlevel.h"

u32 strlen (const u8 *s);
u8 *strcat (u8 *dest, const u8 *src);
u8 *strcpy(u8 *dest, const u8 * src);
u8 *strrev(u8 *buffer);
u8 *ltoa (u32 value, u8 *buffer, u8 radix);

#endif
