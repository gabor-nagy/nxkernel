#include "arch.h"
#include "prot.h"

void segd_change(void *segdt, u16 index, u32 base, u32 limit, u32 flags)
{
   u32 *dt = (u32 *) (segdt + 8 * index);

   dt[0] = (base << 16 & 0xffff0000) | (limit & 0xffff);
   dt[1] = (base & 0xff000000) | (flags & 0xf0ff00) |
            (limit & 0x0f0000) | (base >> 16 & 0xff);
}

u16 segd_create(void *segdt, u32 base, u32 limit, u32 flags)
{
   u16 index;
   u32 *dt = (u32 *) (segdt + 8 * index);

   for (index = 2; dt[index]; index += 2) ;

   if (segdt == (u32 *) GDT_START) {
      if (index > GDT_MAX_INDEX) return 0;
   } else {
      if (index > LDT_MAX_INDEX) return 0;
   }

   segd_change(segdt, index, base, limit, flags);

   return index;
}

void sysd_change(void *sysdt, u16 index, u32 offset, u16 type)
{
  u32 *entry = (u32*) (sysdt + 8 * index);

  entry[0] = (1 << 19) | (offset & 0xffff);		/* TODO: 1 (GDT_CODE_INDEX) magic number */
  entry[1] = (offset & 0xffff0000) | (type & 0xef00);
}

void build_pgd()
{
   u32 *p = (u32 *) PGD_START;
   u32 size_4mb, pgts_start, i;
   
   /* calculate number of 4mb 1st level pgd entries */
   size_4mb = (total_memory / 0x1000) + ( (total_memory % 0x1000 != 0) ? 1 : 0);
   
   /* find first 4k page boundary right after PGD */
   for (pgts_start = PGD_START + size_4mb * 4; pgts_start % 0x1000 != 0; pgts_start += 4);
   pgts_start = PGD_START + size_4mb * 4 + (1024 - size_4mb % 1024) * 4;

   /* build PGD */
   for (i = 0; i < size_4mb; i++)
   {
      *(p++) = (pgts_start + i * 0x1000) | PG_PRESENT | PG_WRITABLE | PG_SUPERVISOR;
   }

   /* build PGTs */
   p = (u32 *) pgts_start;
   for (i = 0; i < total_memory / 4; i++)
   {
      *(p++) = (i * 0x1000) | PG_PRESENT | PG_WRITABLE | PG_SUPERVISOR;
   }

   /* build page stack */
   pgs_start = (u32) p;
   i = total_memory * 0x400;    	 	// i = total memory size in bytes
   while ( (u32) p  < i ) *(p++) = (i -= 0x1000);
   pgs_pointer = --p;
}

u32 free_memory()
{
   return free_pg_cnt() * 4;
}

PRIVATE u32 free_pg_cnt()
{
  return (((u32) pgs_pointer - pgs_start) / 4);
}
