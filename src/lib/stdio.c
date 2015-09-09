#include "stdio.h"

int getchar() /* TODO: not standard! */
{
   int ch;
   
   while (!(ch = getch()));
   
   return ch;
}

void printf(const char *format, ...)
{
   int count = 0;
   int int_data;
   char buffer[10];
   char *str = (char *) format;
   va_list vl;

   va_start(vl, format);
   while (*str != 0)
   {
      if (*str == '%' && *(str+1) == 'd')
      {
         kprintf((char *) ltoa(va_arg(vl, int), buffer, 10));
         str++;
      }
      else if (*str == '%' && *(str+1) == 'x')
      {
         kprintf((char *) ltoa(va_arg(vl, int), buffer, 16));
         str++;
      }
      else if (*str == '%' && *(str+1) == 's')
      {
         kprintf(va_arg(vl, char *));
         str++;
      }
      else
      {
         buffer[0] = *str;
         buffer[1] = '\0';
         kprintf(buffer);
      }

      str++;
   }
   va_end(vl);
}
