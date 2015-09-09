#include "video.h"

u16 get_cursor_pos()
{
   u16 result;

   _out(CRTC_ADDR, 0xe);	/* cursor location high */
   result = in(CRTC_DATA) << 8;
   _out(CRTC_ADDR, 0xf);	/* cursor location low */
   result |= in(CRTC_DATA);
   result <<= 1;
   
   return result;
}

void clear_screen()
{
   memset((void *) VIDEOMEM, VIDEO_EMPTY_SPACE, SCREEN_Y * SCREEN_X, 2);

   cursor_pos = 0;
   sync_video_cursor();
}

void set_xy(u8 x, u8 y)
{
   cursor_pos = 2 * (y * SCREEN_X + x);
   sync_video_cursor();
}

void get_xy(u8 *x, u8 *y)
{
   *x = (cursor_pos / 2) % SCREEN_X;
   *y = (cursor_pos / 2) / SCREEN_X;
}

void scroll()
{
   /* move screen contents one row up */
   memcpy((void *) VIDEOMEM, (void *) VIDEOMEM + SCREEN_X_BYTES, (SCREEN_Y - 1) * SCREEN_X_BYTES);
   /* set last row blank */
   memset((void *) VIDEOMEM + (SCREEN_Y - 1) * SCREEN_X_BYTES, VIDEO_EMPTY_SPACE, SCREEN_X_BYTES, 2);
}

void kprintf(void *m)
{
   u16 *vidmem  = (u16 *) VIDEOMEM;
   u8  *message = (u8 *) m;
   u16 cp       = cursor_pos >> 1;

   while (*message) {
       if (*message == '\n') cp += (SCREEN_X) - (cp % SCREEN_X);
       else if (*message == '\t') cp += VIDEO_TAB_SIZE;
       else vidmem[cp++] = text_attr << 8 | *message;
       
       message++;
       
       if (cp >= SCREEN_X * SCREEN_Y) {

          scroll();

          for (cp = (SCREEN_Y - 1) * SCREEN_X; cp < SCREEN_X * SCREEN_Y; cp++)
             vidmem[cp] = text_attr << 8 | ' ';

          set_xy(0, SCREEN_Y - 1);
          cp = cursor_pos >> 1;
       }
   }
    
   cursor_pos = cp << 1;
   sync_video_cursor();
}

void sync_video_cursor()
{
   _out(CRTC_ADDR, 0xe);		/* cursor location high */
   _out(CRTC_DATA, cursor_pos >> 9);
   _out(CRTC_ADDR, 0xf);		/* cursor location low */
   _out(CRTC_DATA, cursor_pos >> 1 & 0xff);
}
