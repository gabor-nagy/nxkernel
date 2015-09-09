#include "keyboard.h"

#ifdef HW_KEYBOARD_ENABLED

/* read one element from buffer */
int getch()
{
   return (keyboard_head != keyboard_tail) ? keyboard_buffer
   	  [(keyboard_head < KEYBOARD_BUFFER_MAX) ? keyboard_head++ : ((keyboard_head = 0) + KEYBOARD_BUFFER_MAX)]
	     : 0;
}

/* keyboard interrupt handler */
void int_handler_0x29()
{
   register char i, b;
   static char shift_state = 0;
   
   int x,y;
   
   __asm__("pusha");
   
   b = in(0x60);
   
   if ((b & 0x80) == 0)
   {
      i = (keyboard_tail < KEYBOARD_BUFFER_MAX) ? keyboard_tail + 1 : 0;
      
      if (i != keyboard_head)
      {
         switch (b)
	 {
            case 0x2a : /* left shift */
      	    case 0x36 : /* right shift */
	    	 shift_state = ( (shift_state >= 5) ? 6 : 1 );
		 break;
      	    case 0x1d : /* left ctrl */
	    	 shift_state = 2;
		 break;
       	    case 0x38 : /* left alt */
 	    	 shift_state = 3;
		 break;
	    case 0x3a : /* caps lock */
	    	 shift_state = (shift_state ? 0 : 5);
		 break;
	 }
	 
	 if (keyboard_buffer[keyboard_tail] = ascii_table[b][shift_state]) 
	 {
            keyboard_tail = i;
	 }
      }
      else kprintf("#KEYB_BUF_FULL#"); /* TODO: raising an exception would be better */
  }
  else
  {
     switch (b & 0x7f)
     {
        case 0x2a : /* left shift */
        case 0x36 : /* right shift */
        case 0x1d : /* left ctrl */
        case 0x38 : /* left alt */
	     shift_state = ( (shift_state != 6) ? 0 : 5 );
	     break;
     }
  }

  b = in(0x61);

  out(0x61,b | 0x80);
  out(0x61,b);
  out(0x20, 0x20);
  
  __asm__("popa; leave; iret");
}

#endif
