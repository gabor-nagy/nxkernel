#include "shell.h"

int shell_init()
{
   char *ch = " ";

   do
   {
      ch[0] = getchar();
      printf(ch);
   }
   while (1);

   #ifdef DEBUG
   printf("shell_init() OK\n");
   #endif

   return 0;
}
