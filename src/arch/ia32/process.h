#ifndef IA32_PROCESS_H
#define IA32_PROCESS_H

#include "../../sys/macro.h"
#include "../../sys/types.h"
#include "types.h"

struct tss_t
{
   u32 link;

   u32 esp0;
   u32 ss0;
   u32 esp1;
   u32 ss1;
   u32 esp2;
   u32 ss2;

   u32 cr3;

   u32 eip;
   u32 eflags;
   
   u32 eax;
   u32 ecx;
   u32 edx;
   u32 ebx;
   u32 esp;
   u32 ebp;
   u32 esi;
   u32 edi;

   u32 es;
   u32 cs;
   u32 ss;
   u32 ds;
   u32 fs;
   u32 gs;

   u32 ldtr;
   
   u32 iopb_offset;
} ;

/* interface */

tid_t create_thread(pid_t process_id);
int kill_thread(tid_t thread_id);

pid_t create_process();
int kill_process(pid_t process_id);

/* private, hardware-dependent */

PRIVATE u16 create_tss_descriptor();
PRIVATE int create_tss_segment();

#endif
