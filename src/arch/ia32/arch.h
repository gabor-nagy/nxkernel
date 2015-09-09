#ifndef IA32_ARCH_H
#define IA32_ARCH_H

#include "../../sys/config.h"

#include "types.h"
#include "lowlevel.h"
#include "prot.h"
#include "video.h"
#include "keyboard.h"

#define PIT0_TIMERWORD	1193180/SCHE_PER_SEC	/* 8253A PIT timer word */

u32 total_memory;	/* set by init.s, holds the total amount of memory in the system */

u8 arch_init();

PRIVATE void init_pic(u8 new_irq0, u8 new_irq8);
PRIVATE void set_pic_timer(u16 timerword);

PRIVATE void init_gdt();
PRIVATE void init_idt();
PRIVATE void init_video();
PRIVATE void init_paging();

#endif
