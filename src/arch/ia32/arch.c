#include "arch.h"

u8 arch_init()
{
   #ifdef DEBUG
   u8 s[10];
   #endif

   init_video();
   init_gdt();
   init_paging();
   init_idt();
   
   init_pic(0x28, 0x20);
   set_pic_timer(PIT0_TIMERWORD);

   #ifdef HW_KEYBOARD_ENABLED
   out(0x21,0xfd);		/* enable keyboard interrupt and disable everything else */
   #endif
   __asm__("sti");		/* enable interrupts */

   #ifdef DEBUG
   kprintf("\nPGD: ");
   kprintf((u8 *) ltoa(PGD_START / 1024, s, 10));
   kprintf("\nPGS: ");
   kprintf((u8 *) ltoa(pgs_start / 1024, s, 10));
   kprintf("\nTotal memory: ");
   kprintf((u8 *) ltoa(total_memory, s, 10));
   kprintf("\nFree memory: ");
   kprintf((u8 *) ltoa(free_memory(), s, 10));

   kprintf("\n\narch_init() OK\n\n");
   #endif

   return 0;
}

void init_pic(u8 new_irq0, u8 new_irq8)
{
   out(0x20, 0x11);
   out(0xa0, 0x11);		/* ICW1 (ICW4 needed, cascaded PICs) */
   out(0x21, new_irq0);
   out(0xa1, new_irq8);		/* ICW2 (new vector addresses) */
   out(0x21, 0x04);
   out(0xa1, 0x02);		/* ICW3 (establish connection between PICs) */
   out(0x21, 0x01);
   out(0xa1, 0x01);		/* ICW4 (manual EOI, 80x86 environment) */
}

void set_pic_timer(u16 timerword)
{
   out(0x43, 0x36);		/* timer #0, mode 3, write low then high byte */
   out(0x40, timerword & 0xff);
   out(0x40, timerword >> 8);
}

void init_gdt()
{
   u16 *lim  = (u16*) GDT_START;
   u32 *base = (u32*) (GDT_START + 2);

   *lim  = (u16) GDT_SIZE;
   *base = (u32) GDT_START;

   segd_change((void *) GDT_START, 1, 0, 0xfffff, FLAGS_CODE(0) | SEGD_GRAN);     /* code */
   segd_change((void *) GDT_START, 2, 0, 0xfffff, FLAGS_DATA(0) | SEGD_GRAN);     /* data & stack */

   /* stack is already sized, see init.s */

   LOAD_GDTR;

   #ifdef DEBUG
   kprintf("init_gdt() OK\n");
   #endif
}

void int_handler_0x0()
{
  __asm__("pusha");
  kprintf("INT0");
  out(0x20, 0x20);
  __asm__("popa; leave; iret");
}

void int_handler_0x3()
{
  __asm__("pusha");
  kprintf("INT3");
  out(0x20, 0x20);
  __asm__("popa; leave; iret");
}

void int_handler_0xd()
{
  __asm__("pusha");
  kprintf("INT13");
  out(0x20, 0x20);
  __asm__("cli; hlt");
  __asm__("popa; leave; iret");
}

void int_handler_0x28()
{
  __asm__("pusha");
  kprintf("INT28");
  out(0x20, 0x20);
  __asm__("popa; leave; iret");
}

void init_idt()
{
   #ifdef DEBUG
   u8 s[10];
   #endif

   u16 *lim  = (u16 *) IDTR_ADDR;
   u32 *base = (u32 *) (IDTR_ADDR + 2);
   
   *lim = (u16) IDT_SIZE;	/* idt limit */
   *base = (u32) IDT_START;	/* idt base */

   sysd_change((void *) IDT_START, 0x00, (u32)int_handler_0x0, IG_PRESENT);
   sysd_change((void *) IDT_START, 0x03, (u32)int_handler_0x3, IG_PRESENT);
   sysd_change((void *) IDT_START, 0x0d, (u32)int_handler_0xd, IG_PRESENT);
   sysd_change((void *) IDT_START, 0x28, (u32)int_handler_0x28, IG_PRESENT);
   #ifdef HW_KEYBOARD_ENABLED
   sysd_change((void *) IDT_START, 0x29, (u32)int_handler_0x29, IG_PRESENT);
   #endif
   LOAD_IDTR;

   #ifdef DEBUG
   struct idtr { u16 l; u32 b; } __attribute__ ((packed)) r;
   /*asm("sidt %0" : : "m"(r));*/
   kprintf("\nIDT base: ");
   kprintf((u8 *) ltoa(r.b, s, 16));
   kprintf("\nIDT limit: ");
   kprintf((u8 *) ltoa(r.l, s, 16));
   kprintf("\n\ninit_idt() OK\n");
   #endif
   
   //__asm__("cli; hlt");
}

void init_video()
{
   cursor_pos = get_cursor_pos();

   #ifdef DEBUG
   kprintf("init_video() OK\n");
   #endif
}

void init_paging()
{
   build_pgd();

   LOAD_CR3;
   ENABLE_PG;

   #ifdef DEBUG
   kprintf("init_paging() OK\n");
   #endif
}
