#ifndef IA32_TYPES_H
#define IA32_TYPES_H

/* The kernel's hw-dependent parts should only use the following 6 data types. */

/* 1 byte */
typedef unsigned char u8;
typedef signed   char s8;

/* 2 bytes */
typedef unsigned short int u16;
typedef signed   short int s16;

/* 4 bytes */
typedef unsigned long u32;
typedef signed   long s32;

#endif
