#ifndef IA32_VIDEO_H
#define IA32_VIDEO_H

#include "../../sys/config.h"
#include "lowlevel.h"
#include "memory.h"

#define VIDEOMEM 0xb8000
#define SCREEN_X 80
#define SCREEN_Y 25

#define VIDEO_TAB_SIZE 8
#define SCREEN_X_BYTES (2 * SCREEN_X)     /* number of bytes in a row */

#define CRTC_ADDR 0x3d4
#define CRTC_DATA 0x3d5 /* VGA CRT Controller register pair */

u16 cursor_pos;	        /* holds cursor position (next address to write in video memory) */
u16 text_attr = 0x07;   /* text attributes: background and foreground color */

#define VIDEO_EMPTY_SPACE (text_attr << 8 | ' ')

u16 get_cursor_pos();
void clear_screen();

void set_xy(u8 x, u8 y);
void get_xy(u8 *x, u8 *y);

void scroll();

void kprintf(void *m);

static void sync_video_cursor();

#endif
