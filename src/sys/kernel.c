#include "kernel.h"

int main()
{
   arch_init();
   shell_init();

   printf("\nmemory: %x %x %x %x %x %x %x %x\n",
      *((unsigned char *)0xac4+0),*((unsigned char *)0xac4+1), *((unsigned char *)0xac4+2),
      *((unsigned char *)0xac4+3),*((unsigned char *)0xac4+4),*((unsigned char *)0xac4+5),
      *((unsigned char *)0xac4+6),*((unsigned char *)0xac4+7));
   
   /*u16 *lim  = (u16 *) IDTR_ADDR;
   u32 *base = (u32 *) (IDTR_ADDR + 2);

   printf("\nidtr: %x %x\n", *lim, *base);
   printf("idt[0x29]: %x %x %x %x\n",
      *((u32*) (IDT_START + 0x29 * 8)), *((u32*) (IDT_START + 0x29 * 8 + 4)));*/
      
   //__asm__("int $0x0");

   printf("\nKernel halted.");

   return 0;
}
