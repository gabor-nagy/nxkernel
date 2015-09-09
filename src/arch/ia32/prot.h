#ifndef IA32_PROT_H
#define IA32_PROT_H

#include "types.h"
#include "../../sys/config.h"
#include "../../sys/macro.h"

/* segment descriptor symbols */

#define SEGD_           0x001000                /* this is a segment descriptor */
#define SEGD_GRAN       (0x800000 | SEGD_)	/* granularity */
#define SEGD_16         (0        | SEGD_)	/* 16-bit segment */
#define SEGD_32         (0x400000 | SEGD_)	/* 32-bit segment */
#define SEGD_PRESENT    (0x008000 | SEGD_)	/* segment is in memory */
#define SEGD_NOTPRESENT (0        | SEGD_)	/* not in memory */
#define SEGD_CODE       (0x000800 | SEGD_)	/* segment contains code */
#define SEGD_DATA       (0        | SEGD_)	/* segment contains data */
#define SEGD_CONF       (0x000400 | SEGD_)	/* conforming code */
#define SEGD_NONCONF    (0        | SEGD_)	/* non-conforming code */
#define SEGD_EXPDOWN    (0x000400 | SEGD_)	/* stack expands down */
#define SEGD_READABLE   (0x000200 | SEGD_)	/* code is readable */
#define SEGD_WRITABLE   (0x000200 | SEGD_)	/* data is writable */

#define SEGD_RING0	(0        | SEGD_)
#define SEGD_RING1	(0x002000 | SEGD_)
#define SEGD_RING2	(0x004000 | SEGD_)
#define SEGD_RING3	(0x006000 | SEGD_)	/* desc privilege levels */

#define FLAGS_CODE(L)	(SEGD_PRESENT | SEGD_32 | SEGD_CODE | SEGD_RING ## L)
#define FLAGS_DATA(L)	(SEGD_PRESENT | SEGD_32 | SEGD_DATA | SEGD_WRITABLE | SEGD_RING ## L)
#define FLAGS_STACK(L)	(SEGD_PRESENT | SEGD_32 | SEGD_DATA | SEGD_WRITABLE | SEGD_RING ## L)

/* interrupt gates */
#define	IG_		0x0e00
#define	IG_PRESENT	0x8000 | IG_
#define	IG_TRAP		0x0100 | IG_

/* common selector symbols */

#define SEL_RING0	0
#define SEL_RING1	1
#define SEL_RING2	2
#define SEL_RING3	3
#define SEL_TI		4

/* page entry symbols */

#define	PG_PRESENT	0x0001			/* page is in memory */
#define	PG_WRITABLE	0x0002			/* page is write-enabled */
#define	PG_SUPERVISOR	0x0004			/* can be accessed from ring 0,1,2 only */
#define	PG_PWT		0x0008			/* Page-Write-Through */
#define	PG_PCD		0x0010			/* Page Cache Disable */
#define	PG_INUSE	0x0200			/* page is used by another process */

/* limit macros */

#define IDTR_ADDR	0x100000		/* IDT register address */
#define IDT_START	0x100008
#define IDT_SIZE	0x7f8			/* 2k-8 size */
#define GDT_START	0x100800                /* 1M + 2k */
#define GDT_SIZE	0x400			/* 1k size */
#define PGD_START	0x101000                /* 1M + 8k (must be on 4k boundary) */
#define GDT_MAX_INDEX	(GDT_SIZE / 8 - 2)      /* entry 0 reserved */

#define LDT_SIZE	MIN(PAGE_SIZE, 0x10000) /* one page will be reserved for each LDT */
#define LDT_MAX_INDEX	(LDT_SIZE / 8 - 2)      /* we use entry 0 in LDT for NULL pointers */

/* macros */

#define LOAD_GDTR	__asm__ ("lgdt (%0)" : : "r"(GDT_START)) /* load GDTR */
#define LOAD_IDTR	__asm__ ("lidt (%0)" : : "r"(IDTR_ADDR)) /* load IDTR */
#define LOAD_CR3	__asm__ ("mov %0, %%eax;" \
				 "mov %%eax, %%cr3" : : "c"(PGD_START) : "%eax") /* load CR3 */
#define ENABLE_PG	__asm__ ("mov %%cr0, %%eax\n\t" \
				 "or $0x80000000, %%eax\n\t" \
				 "mov %%eax, %%cr0" : : : "eax") /* enable paging */
				 
/* global variables */

u32 pgs_start;		/* set by build_pgd(); */
u32 * pgs_pointer;	/* set by build_pgd(); */

/* prototypes */

void segd_change(void *segdt, u16 index, u32 base, u32 limit, u32 flags);
u16  segd_create(void *segdt, u32 base, u32 limit, u32 flags);
void sysd_change(void *sysdt, u16 index, u32 offset, u16 type);

void build_pgd();

u32 free_memory(); /* free memory in KB */

PRIVATE u32 free_pg_cnt();

#endif
