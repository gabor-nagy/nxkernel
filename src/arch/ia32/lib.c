#include "lib.h"

int close(int file)
{
   return -1;
}

int execve(char *name, char **argv, char **env)
{
   /*errno=ENOMEM;*/
   return -1;
}

int fork()
{
   /*errno=EAGAIN;*/
   return -1;
}

int fstat(int file, struct stat *st)
{
   /*st->st_mode = S_IFCHR;*/
   return 0;
}

int getpid()
{
   return 1;
}

int isatty(int file)
{
   return 1;
}

int kill(int pid, int sig)
{
  /*errno=EINVAL;*/
  return(-1);
}

int link(char *old, char *new)
{
  /*errno=EMLINK;*/
  return -1;
}

int lseek(int file, int ptr, int dir)
{
   return 0;
}

int open(const char *name, int flags, int mode)
{
   return -1;
}

int read(int file, char *ptr, int len)
{
   return 0;
}

/*caddr_t*/ unsigned long sbrk(int incr)
{
   /*extern char end;		/ Defined by the linker /
   static char *heap_end;
   char *prev_heap_end;
   
   if (heap_end == 0)
   {
      heap_end = &end;
   }
   
   prev_heap_end = heap_end;
   
   if (heap_end + incr > stack_ptr)
   {
      _write (1, "Heap and stack collision\n", 25);
      abort();
   }
   
   heap_end += incr;
   return (caddr_t) prev_heap_end; */
}

int stat(char *file, struct stat *st)
{
   /*st->st_mode = S_IFCHR;*/
   return 0;
}

int times(struct tms *buf)
{
   return -1;
}

int unlink(char *name)
{
   /*errno=ENOENT;*/
   return -1;
}

int wait(int *status)
{
   /*errno=ECHILD;*/
   return -1;
}

int write(int file, char *ptr, int len)
{
   /*int todo;
   for (todo = 0; todo < len; todo++)
   {
      writechar(*ptr++);
   }
   return len; */
}
