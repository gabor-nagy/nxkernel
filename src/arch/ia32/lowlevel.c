#include "lowlevel.h"

inline u8 in(u16 port)
{
   u8 result;
   _in(port, result);
   return result;
}

inline u8 out(u16 port, u8 data)
{
   _out(port, data);
   return data;
}

inline void memcpy(void *dest, void *source, u32 count)
{
   __asm__("cld");
   
   switch (count & 0x3)
   {
      case 0:    /* 32-bit copy */
         __asm__ volatile ("rep movsl" : : "S" (source), "D" (dest), "c" (count >> 2) : "memory");
         break;
      case 2:    /* 16-bit copy */
         __asm__ volatile ("rep movsw" : : "S" (source), "D" (dest), "c" (count >> 1) : "memory");
         break;
      default:   /* 8-bit copy  */
         __asm__ volatile ("rep movsb" : : "S" (source), "D" (dest), "c" (count)      : "memory");
         break;
   }
}

inline void memset(void *buffer, u32 value, u32 count, u8 value_size)
{
   __asm__("cld");
   
   switch (value_size & 0x3)
   {
      case 0:    /* 32-bit fill */
         __asm__ volatile ("rep stosl" : : "D" (buffer), "a" (value), "c" (count) : "memory");
         break;
      case 2:    /* 16-bit fill */
         __asm__ volatile ("rep stosw" : : "D" (buffer), "a" (value), "c" (count) : "memory");
         break;
      default:   /* 8-bit fill  */
         __asm__ volatile ("rep stosb" : : "D" (buffer), "a" (value), "c" (count)      : "memory");
         break;
   }
}
