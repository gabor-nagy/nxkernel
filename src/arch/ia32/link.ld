OUTPUT_FORMAT("binary")

ALIGN_VAL = 0x400;

SECTIONS
{
   .text 0x500 : {
      code = .; _code = .; __code = .;
      *(.text)
      . = ALIGN(ALIGN_VAL);
   }
  
   .data  : {
      data = .; _data = .; __data = .;
      *(.data)
      . = ALIGN(ALIGN_VAL);
   }

  end = .; _end = .; __end = .;
}
