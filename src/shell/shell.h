#ifndef SHELL_H
#define SHELL_H

/* choosing shell */

#if (SHELL == CLSH)
   #include "clsh/shell.h"
#else
   #include "Unknown shell. Maybe incorrect 'sys/config.h' SHELL setting."
#endif

#endif
