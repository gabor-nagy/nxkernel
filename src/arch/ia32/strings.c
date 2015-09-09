#include "strings.h"

u32 strlen (const u8 *s)
{
   u32 result = 0;

   while (*s) ++result, ++s;
      
   return result;
}

u8 *strcat (u8 *dest, const u8 *src)
{
   memcpy(dest + strlen(dest), (void *) src, strlen(src) + 1);
   
   return dest;
}

u8 *strcpy(u8 *dest, const u8 * src)
{
   u8 *d = dest;
   
   while (*src) *dest++ = *src++;
   
   *dest = '\0';
   
   return d;
}

u8 *strrev(u8 *buffer)
{
   u8 temp;
   u8 *s = buffer;
   u32 i;
   u32 length;
   
   length = strlen(s);
   
   for (i = 0; i < length / 2; i++)
      {
	 temp = s[i];
	 s[i] = s[length - i - 1];
	 s[length - i - 1] = temp;
      }
      
   return buffer;
}

u8 *ltoa (u32 value, u8 *buffer, u8 radix)
{
   u8 numbers[] = "0123456789abcdef";
   u8 *s = buffer;
   u16 i = 0;

   while (value)
   {
     s[i++] = numbers[value % radix];
     value = value / radix;
   }
   
   if (i == 0) s[i++] = '0';
      
   s[i] = 0;
   
   return strrev(buffer);
}
