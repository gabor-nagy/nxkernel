#ifndef ARCH_IA32_LIB_H
#define ARCH_IA32_LIB_H

/* C library interface implementation
 * ----------------------------------
 * Consistent with RedHat's newlib
 * http://sourceware.org/newlib/libc.html#SEC195
 */
 
#include "../../lib/errno.h"

struct stat { } ;
struct tms { } ;

char *__env[1] = { 0 };
char **environ = __env;

int close(int file);
int execve(char *name, char **argv, char **env);
int fork();
int fstat(int file, struct stat *st);
int getpid();
int isatty(int file);
int kill(int pid, int sig);
int link(char *old, char *new);
int lseek(int file, int ptr, int dir);
int open(const char *name, int flags, int mode);
int read(int file, char *ptr, int len);
/*caddr_t*/ unsigned long sbrk(int incr);
int stat(char *file, struct stat *st);
int times(struct tms *buf);
int unlink(char *name);
int wait(int *status);
int write(int file, char *ptr, int len);

#endif
