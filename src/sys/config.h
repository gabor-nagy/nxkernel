#ifndef CONFIG_H
#define CONFIG_H

#define VERSION 0.1     /* kernel version */

#define ARCH IA32       /* architecture type, only IA32 supported yet (Intel 80386, AMD K6, Cyrix 5x86 and later) */
#define SHELL CLSH	/* initial shell to use */
#define DEBUG		/* if defined, kernel prints debug information */

/* advanced settings */

#define SCHE_PER_SEC 50	/* number of scheduling algorithm runs per second */
#define PAGE_SIZE 4096  /* the smallest unit of memory allocation, must be a power of 2 */
#define PAGE_SHIFT 12   /* log2 of the above */

/* hardware components */
#define HW_KEYBOARD_ENABLED

#endif
