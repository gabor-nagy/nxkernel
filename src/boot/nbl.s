; --------------------------------------------------
; Nax Boot Loader 1.06.1
; Created by Gabor Nagy, 2003-2006
; --------------------------------------------------
; This software is part of the NxMP OS project.
; Released under the Reciprocal Public License v1.1
; See license.txt for more information.

; HISTORY
; -------
; 1.00.1 (04.07.23-30)	- first stable version of NBL multistage boot loader
;
; 1.01.1 (04.07.31)	- basic MULTIBOOT implementation, kills drive motor
;
; 1.02.1 (04.07.31)	- verifies A20 address line after trying to enable it
;	 (04.08.01)	- added fast mode to enable A20 line
;			- bugfix: initialize si before calling 'get_mem_size'
;			- added int12 to 'get_mem_size'
;			- parameter block now includes 'current_drive'
;			- can be loaded from HDD too (cyl num is limited to 255)
;			- 1st stage code optimizations
;
; 1.03.1 (04.08.04)	- BUGFIX: enable_a20 was a mess... I think I must have
;			  been sleeping when I wrote it :-) now it's working
;
; 1.04.1 (05.03.13)	- final kernel address can now be defined (KERNEL_COPYTO symbol)
; 	 (05.03.15)	- BUGFIX: descriptors' G bit wasn't set, so limit was 1MB instead of 4GB
;
; 1.05.1 (05.03.27)	- BUGFIX: incorrect kernel size calculation
; 	 		  (attempts to access real mode variables from pmode)
;			- BUGFIX: wtitenum didn't print zeros other than ending zeros
;        (05.03.28)	- BUGFIX: new tested LBA -> CHS method (old one didn't match physical structure)
;	 		- can now correctly load kernels up to 512kb in size
;        (05.07.14)     - got rid of nasm warning messages (res? x -> times x res? 0)
;        (05.07.29)	- new kernel load address is now 0x4000 (+ added warning comment)
;                       - new copyright notices
;	 (05.08.08)	- kernel loads to 0x500 (KERNEL_SEG and NBL_CODE_SEG also modified)
;
; 1.06.1 (06.02.18)	- compilation produces exect code size (currently 2k)
;			- return conventional memory size in multiboot block
;			- VERSION macro, string optimizations
;

[BITS 16]
[CPU 386]

%define VERSION "1.06.1"

NL			equ	10	; constant for new line
NBL_CODE_SEG		equ	0x8f00	; seg where to load 2nd stage loader
KERNEL_SEG		equ	0x51	; seg where to load kernel at first - DON'T HURT 0x413 (conv. memory size)
KERNEL_COPYTO		equ	0x500	; absolute address of where to copy kernel code in the end
                                        ; WARNING: this value must mach the one used in link.ld script

		struc boot_info		; boot information block (MULTIBOOT STANDARD)
.flags:		resd	1
.mem_lower:	resd	1
.mem_upper:	resd	1
		endstruc

;*******************************************************************************
;***                               1st STAGE                                 ***
;*******************************************************************************

	jmp	start			; jump over parameter block

;- PARAMETER BLOCK ------------------------------------

pb_length		db	PB_SIZE	; total length of parameter block
stage2_length_1		db	3	; 2nd stage loader length in sectors (max. 18)

sec_per_track_1		db	18	; for my bochs test drive that is 17
side_per_track_1	db	2	;                                 615
bytes_per_sector_1	dw	512
current_drive_1		db	0	; drive where NBL was loaded from (NBL sets it)

kernel_map_sec1_1	db	4	; kernel map linear sector 1 (must be >0)
kernel_map_sec2_1	db	0	; kernel map linear sector 2 (0 if not used)
kernel_map_sec3_1 	db	0	; kernel map linear sector 3 (0 if not used)
kernel_map_sec4_1	db	0	; kernel map linear sector 4 (0 if not used)

PB_SIZE equ ($-$$)-3

;- CODE -----------------------------------------------

start:
	mov	ax,0x7C0
	mov	ds,ax			; set up DS
	mov	ss,ax			
	mov	sp,1024			; setup stack

	mov	[current_drive_1],dl	; store which drive NBL was loaded from
					; BIOS sets dl to this value on entry

	mov	ax,3
	int	0x10			; set text mode 80x25, color

	mov	bx,0xb800
	mov	es,bx
	xor	bx,bx			; es:bx -> screen memory
	mov	cx,40
	mov	ax,0xf00
	rep	stosw			; set white foreground color for first row

	mov	si,str_init_first_stage
	call	writestr_1		; print init string
	call	dottedline_1

	mov	ax,NBL_CODE_SEG
	mov	es,ax			; es = seg to load 2nd stage code
	xor	bx,bx			; bx = 0
	mov	ax,1			; sector 2 (linear 0)
	call	linconv_1
	mov	al,[stage2_length_1]	; al = sectors to read
	mov	bp,cx			; save cx to bp
	
	mov	cx,3			; try reading at least 3 times
.try:
	pusha				; ax will be destroyed by int handler
	xor     ax,ax
        int     0x13                     ; reset disk system
	popa
	push	ax
	xchg	cx,bp
	int	0x13			; read, return CF set if error
	xchg	cx,bp
	pop	ax
	jnc	.read_ok
	loop	.try
	
.error:
	mov	si,str_error
	call	writestr_1

	xor     ax,ax
        int     0x16			; wait for keypress
        jmp	0xffff:0		; reboot

.read_ok:
	mov	si,str_ok
	call	writestr_1

	mov	di,NBL_CODE_SEG
	mov	es,di
	mov	di,sec_per_track	; es:di -> NBL_CODE_SEG:3
	mov	si,sec_per_track_1	; ds:si -> ds:5
	movzx	cx,[pb_length]
	sub	cx,2
	rep	movsb			; copy parameter block up for 2nd stage loader

	jmp	NBL_CODE_SEG:0		; start 2nd stage loader

; ----------
; writestr_1
; ----------
; Writes null-terminated string on screen.
;
;	[ds:si]		null-terminated string to write
;
; Destroys:	ax,bx
;

writestr_1:

	mov     ah,0xE			; ROM-BIOS service 0x0E
        xor     bx,bx			; screen page = 0
.nextch:
	lodsb				; AL = next character
        and     al,al			; if AL = 0 (EOS) then
        jz      .exit			; jump to exit
.int:	int     0x10			; print char
	cmp	al,NL			; if AL <> NL then
	jne	.nextch			; jump
	add	al,3			
	jmp	.int			; else print 0Dh

.exit:	ret

; -------------
; dottedline_1
; -------------
; Prints a dotted line ending in screen coloumn 70.
;
; Destroys:	-

dottedline_1:

	xor	bx,bx
	mov	ax,0xe*256+" "		; print an empty space
	int	10h
.nextdot:
	mov	ax,0xe*256+"."		; print a dot
	int	0x10

	mov	ah,0x3			; get current coloumn
	int	0x10
	cmp	dl,70
	jl	.nextdot		; if less than 70, loop

	mov	ax,0xe*256+" "		; print an empty space
	int	0x10

.exit:	ret

; ---------
; linconv_1
; ---------
; Converts linear sector addresses to 3D addresses.
;
;	ax	linear address
;
; Returns:	dl = current drive
;		dh = side
;		ch = track
;		cl = sector
;		ah = service 2 (read)
;		al = * (sectors to read)
;		es:bx * ? (buffer)
;

linconv_1:

	push	bx
	push	ax

	div	BYTE [sec_per_track_1]
	inc	ah
	mov	cl,ah			; cl = sector
	xor	ah,ah
	div	BYTE [side_per_track_1]
	mov	ch,ah
	mov	dh,al
	mov	dl,[current_drive_1]

	pop	ax
	mov	ah,2

	pop	bx

	ret

;- DATA -----------------------------------------------

str_init_first_stage:
db      "þ NBL ", VERSION, NL
db      "  (c) 2003-2006, Gabor Nagy", NL
times 27 db "Ä"
db	NL
db	"Loading 2nd stage",0

str_error:
db	"ERROR",0

str_ok:
db	"OK",NL,0

;- RESERVED -------------------------------------------

times	512-2-($-$$) db 0
dw	0xAA55


;*******************************************************************************
;***                               2nd STAGE                                 ***
;*******************************************************************************

section stage2_code vstart=0

	jmp	start2nd

;- PARAMETER BLOCK ------------------------------------

sec_per_track		db	0
side_per_track		db	0
bytes_per_sector 	dw	0
current_drive		db	0

kernel_map_sec1		db	0	; kernel map linear sector 1 (must be >0)
kernel_map_sec2		db	0	; kernel map linear sector 2 (0 if not used)
kernel_map_sec3 	db	0	; kernel map linear sector 3 (0 if not used)
kernel_map_sec4		db	0	; kernel map linear sector 4 (0 if not used)

;- VARIABLES ------------------------------------------

current_kernel_seg	dw	0	; current segment while loading kernel binary
current_kernel_ofs	dw	0
loaded_kernel_size	dw	0	; counts how many sectors are loaded

boot_info_block:
                times 3 dd	0	; boot info block (MULTIBOOT STANDARD)


;- INITIAL GDT ----------------------------------------

GDT_start:				; 48-bit address of GDT stored in NULL descriptor
dw	0x18				; limit
dd	NBL_CODE_SEG*0x10 + GDT_start	; base
times 2 db 0				; NULL descriptor

; initial code descriptor
dw	0xffff				; limit = 0FFFFFh
dw	0h				; base  = 0h
db	0h
db	10011010b			; present, readable, non-conforming code segment, ring0
db	0xcf				; 32-bit segment
db	0

; initial data descriptor
dw	0xffff				; limit = 0FFFFFh
dw	0h				; base  = 0h
db	0h
db	10010010b			; present, writable, data segment, ring0
db	0xcf				; 32-bit segment
db	0

;- CODE -----------------------------------------------

start2nd:

	mov	ax,NBL_CODE_SEG
	mov	ds,ax
	mov	es,ax
	mov	fs,ax
	mov	ss,ax
	mov	esp,0xffff		; setup new stack

	mov	eax,1
	mov	DWORD [boot_info_block+boot_info.flags],eax

	cli				; disable interrupts (int table will be overwritten!)

	mov	si,msg_a20		; "A20 line: "
	call	writestr
	call	dottedline
	call	enable_a20		; enable A20 address line
	jne	.a20_ok
	mov	si,msg_error		; "ERROR"
	call	writestr
	jmp	$			; hang
.a20_ok:
	mov	si,msg_ok		; "OK"
	call	writestr


	mov	si,msg_mem		; "Memory size: "
	call	writestr
	call	dottedline
	mov	si,system_memory_map	; in case memory is detected by using in 0c7h
	call	get_mem_size		; get extended memory size
	mov	DWORD [boot_info_block+boot_info.mem_upper],eax

	jnc	.mem_ok			; if no error, 
	mov	si,msg_unknown		; "?"
	call	writestr
.mem_ok:
	shr	eax,10			; get size in MB
	call	writenum		; print detected memory size
	mov	si,msg_MB
	call	writestr		; "MB"
	
	int	0x12			; get conventional memory size in ax
	and	eax, 0xffff
	cmp	ax, 640			; don't let bios deceive us
	jle	.store_mem_lower
	mov	ax, 640
.store_mem_lower:
	mov	DWORD [boot_info_block+boot_info.mem_lower],eax

	; reading kernel map
	; ------------------

	mov	si,msg_kernel_read_map	; "Reading kernel map"
	call	writestr
	call	dottedline
	
	mov	bp,kernel_map_sec1	; bp -> first map sector offset
.next_map:
	mov	al,[bp]			; al = next map sector
	and	al,al
	jz	.map_end		; if map sector address == 0, then it's empty

	and	ax,0xff			; only al contains valuable information
	mov	bx,map_start		; es:bx -> map start
	call	read1sector

	mov	si,map_start		; fs:si -> map start
.next_entry:
	lodsw				; ax = get next address
	and	ax,ax
	jz	.map_end		; if entry == 0, end of map

	mov	bx,map_start
	add	bx,[bytes_per_sector]	; es:bx -> kernel_buffer
	call	read1sector		; read kernel sector
	
	jnc	.no_error		; if error, print message and halt
	mov	esi,msg_error
	call	writestr
	hlt
.no_error:
	inc	WORD [loaded_kernel_size]

	pusha
	push	es
	mov	ax,KERNEL_SEG
	add	ax,[current_kernel_seg]
	mov	es,ax			; es = current kernel segment
	mov	di,[current_kernel_ofs]	; di = current offset
	mov	si,map_start
	mov	cx,[bytes_per_sector]
	add	si,cx			; ds:si -> kernel buffer

	rep	movsb			; copy 1 sector from kernel buffer

	cmp	di,0xffff		; if segment is full, jump to next
	jne	.seg_not_full
	mov	ax,0x1000		; 0x1000 * 16 (paragraph) == 64k
	add	[current_kernel_seg],ax	; add this to current_kernel_seg
	xor	ax,ax
	mov	[current_kernel_ofs],ax	; current_kernel_ofs = 0
	jmp	.copy_exit
.seg_not_full:
	mov	ax,[bytes_per_sector]
	add	[current_kernel_ofs],ax	; if seg not full, current_kernel_ofs += byte_per_sec
.copy_exit:
	pop	es
	popa

	mov	ax,si
	sub	ax,map_start
	cmp	ax,[bytes_per_sector]	; if end of sector, read next
	jl	.next_entry

	inc	bp			; next map sector addr
	cmp	bp,kernel_map_sec4	; there are only 4 map sectors max.
	jg	.map_end
	jmp	.next_map

.map_end:

	mov	si,msg_ok		; "OK"
	call	writestr

	mov	dx,0x3f2
	xor	ax,ax
	out	dx,ax			; turn floppy drive motor off

	mov     bp,[bytes_per_sector]   ; we'll need it after switching to pmode
	mov	bx,[loaded_kernel_size]	; like that

	; switching to protected mode
	; ---------------------------

	lgdt	[GDT_start]		; load GDT register

	mov	eax,CR0
	or	eax,1
	mov	CR0,eax			; PE = 1, protected mode
	jmp	.clear_pref			; clear prefetch queue
.clear_pref:
	mov	ax,16
	mov	ds,ax
	mov	es,ax
	mov	fs,ax
	mov	gs,ax
	mov	ss,ax			; set up segments

	mov	esi,KERNEL_SEG * 0x10	; esi = kernel binary
	mov	edi,KERNEL_COPYTO	; edi = where to copy kernel code
	mov	ax,bp			; bytes_per_sector
	mul	bx			; loaded_kernel_size
	mov	cx,dx
	shl	ecx,16
	mov	cx,ax			; ecx = kernel size in bytes
	rep	movsb			; copy kernel down
	
	mov	eax,0x2BADB002		; MULTIBOOT STANDARD 'magic value'

	mov	ebx,NBL_CODE_SEG * 0x10 + boot_info_block

	jmp	8:KERNEL_COPYTO	     	; jump to kernel code

; --------------
; keyb_wait_obf
; --------------
; Waits for the output buffer in the keyboard controller to be empty.
;
; Destroys:	-
;

keyb_wait_obf:

	push	ax
.full:
	in	al,0x64
	and	al,1
	jnz	.full

	pop	ax
	ret

; -----------
; enable_a20
; -----------
; Enables A20 address line by trying different methods.
; A lot of thanks for Chris Giese <geezer@execpc.com> for his PD code "a20.asm"
;
; Destroys:	ax
;
; Returns:	ZF	cleared on error

enable_a20:

	mov	ax,2401
	int	0x15			; try using BIOS
	adc	ah,0			; CF set if error
	and	ah,ah
	jnz	.try_keyb		; if not successful, try keyboard mode
	call	verify_a20
	je	.try_keyb
	jmp	.exit

.try_keyb:				; try using keyboard controller
	call	keyb_wait_obf
	mov	al,0xd0			; command: read output port
	out	0x64,al

.empty:
	in	al,0x64
	and	al,1
	jz	.empty			; wait for IBF high
	in	al,0x60			; read output port

	mov	ah,al
	or	ah,10b			; set 'A20 gate enable' bit

	call	keyb_wait_obf
	mov	al,0xd1			; command: write output port
	out	0x64,al
	call	keyb_wait_obf
	mov	al,ah
	out	0x60,al
	call	keyb_wait_obf		; wait for enabling line

	call	verify_a20		; it might be necessary to use fast mode, too
	jne	.exit

.try_fast:				; try 'fast' mode using port 92h
	in	al,0x92			; port 92h bit 1 is set if A20 is enabled
	test	al,2

	jnz	.try_fast_v		; if bit is already set, exit (may be a CHIPSET bug)
	or	al,2
	out	0x92,al			; set bit 1 to enable A20
.try_fast_v:
	call	verify_a20		; set ZF

.exit:	ret

; -----------
; verify_a20
; -----------
; Verifies A20 address line.
;
; Destroys:	ax
;
; Returns:	ZF	set on error

verify_a20:

	push	es
	push	ds

	xor	ax,ax
	mov	ds,ax
	dec	ax
	mov	es,ax
	mov	ax,WORD [es:0x10]	; ax = [FFFF:0x10] (first byte above 1MB address mark)
	not	ax
	mov	WORD [ds:0],ax		; [0000:0] = not ax
	cmp	ax,WORD [es:0x10]	; if ax == [FFFF:0x10] then A20 is not enabled, ZF=1

	pop	ds
	pop	es
	ret

; -------------
; get_mem_size
; -------------
; Gets extended memory size via BIOS interrupts.
;
;	es:di	20-byte memory buffer (### NOT IMPLEMENTED YET)
;	ds:si	42-byte memory map buffer
;
; Destroys:	ebx
;
; Returns:	eax	extended memory size in K
;		CF	set if error

get_mem_size:

	;mov	eax,0xe820		; get system memory map
	;mov	edx,'SMAP'		; 0x534D4150
	;xor	ebx,ebx
	;mov	ecx,20
	;int	0x15			; ### NOT IMPLEMENTED YET
	
.try_e801:
	mov	ax,0xe801		; get memory size for >64M sytems
	int	0x15
	jc	.try_c7			; if not supported, try service 0xc7
	and	ax,ax			; if ax==0, ax=cx, bx=dx
	jnz	.ax_bx
	mov	ax,cx
	mov	bx,dx
.ax_bx:	and	eax,0xffff
	and	ebx,0xffff
	shl	ebx,6
	add	eax,ebx
	and	ebx,ebx			; if memory is below 16MB, try 286+ BIOS routine
	jz	.try_88
	jmp	.exit_ok		; OK

.try_88:
	mov	ah,0x88
	int	0x15
	jc	.try_int12		; CF state is UNRELIABLE!!!
	and	eax,0xffff
	jmp	.exit_ok
.try_int12:				; try int 0x12 (most likely to be using CMOS)
	int	0x12
	and	eax,0xffff
	jmp	.exit_ok

.try_c7:
	mov	ah,0xc0			; get configuration
	int	0x15
	jc	.try_cmos
	mov	al,[es:bx+06]
	test	al,10000b
	jz	.try_cmos		; see if function 0c7 is supported
	
	mov	ah,0xc7			; return memory-map info
	int	0x15
	jc	.try_cmos
	mov	eax,DWORD [ds:si+2]
	add	eax,DWORD [ds:si+6]
	jmp	.exit_ok		; OK

.try_cmos:
	mov	al,0x18
	out	0x70,al
	in	al,0x71
	mov	ah,al
	mov	al,0x17
	out	0x70,al
	in	al,0x71			; ax = extended memory size in K
	and	eax,0xffff
	jz	.try_88			; if no extended memory, try service 0x88
	jmp	.exit_ok

.exit_ok:
	clc
.exit:	
	ret

; ----------
; writestr
; ----------
; Writes null-terminated string on screen.
;
;	[ds:si]		null-terminated string to write
;
; Destroys:	ax,bx
;

writestr:

	push	ax
	push	bx

	mov     ah,0xe			; ROM-BIOS service 0xe
        xor     bx,bx			; screen page = 0
.nextch:
	lodsb				; AL = next character
        and     al,al			; if AL = 0 (EOS) then
        jz      .exit			; jump to exit
.int:	int     0x10			; print char
	cmp	al,NL			; if AL <> NL then
	jne	.nextch			; jump
	add	al,3
	jmp	.int			; else print 0xd

.exit:	pop	bx
	pop	ax

	ret

; ---------
; writenum
; ---------
; Prints a 32-bit number on the screen. Jumps over leading zeros.
;
;	eax	number to write
;
; Destroys:	-

writenum:

	pusha

	mov	ecx,esp			; store old esp position (without digits pushed)
	xor	di,di

	and	eax,eax
	jnz	.not0
	mov	ax,0xe * 256 + "0"
	push	ax
	jmp	.print			; if eax==0, print a zero digit only

.not0:
	mov	ebx,10			; edx:eax will be divided with 10
.nextdigit:
	xor	edx,edx
	div	ebx			; dx = edx:eax % 10, eax = edx:eax / 10

	and	di,di
	jnz	.nextdigit
	jmp	.store2
.store:	
	mov	di,1			; if di<>0, from now print all 0's
.store2:
	add	dl,"0"			; convert to ascii
	mov	dh,0xe			; print charcter function for int10h
	push	dx

	and	eax,eax
	jnz	.nextdigit		; while eax<>0, extract next digit

.print:
	xor	bx,bx			; screen page 0
.popnext:
	pop	ax			; get next digit
	int	0x10			; print char
	cmp	esp,ecx
	jl	.popnext		; do while there are digits on stack

.exit:	popa

	ret

; -----------
; dottedline
; -----------
; Prints a dotted line ending in screen coloumn 70.
;
; Destroys:	-

dottedline:

	push	ax
	push	bx
	push	cx
	push	dx

	xor	bx,bx
	mov	ax,0xe * 256 + " "	; print an empty space
	int	0x10
.nextdot:
	mov	ax,0xe * 256 + "."	; print a dot
	int	0x10

	mov	ah,3			; get current coloumn
	int	0x10
	cmp	dl,70
	jl	.nextdot		; if less than 70, loop

	mov	ax,0xe * 256 + " "	; print an empty space
	int	0x10

.exit:	pop	dx
	pop	cx
	pop	bx
	pop	ax

	ret

; ---------
; linconv
; ---------
; Converts linear sector addresses to 3D addresses.
;
;	ax	linear address
;
; Returns:	dl = current drive
;		dh = side
;		ch = track
;		cl = sector
;		ah = service 2 (read)
;		al = * (sectors to read)
;		es:bx * ? (buffer)
;

linconv:

	push	bx
	push	ax

	div	BYTE [sec_per_track]
	inc	ah
	mov	cl,ah			; cl = sector
	xor	ah,ah
	div	BYTE [side_per_track]
	mov	ch,al
	mov	dh,ah
	mov	dl,[current_drive]

	pop	ax
	mov	ah,2

	pop	bx

	ret

; ------------
; read1sector
; ------------
; Read 1 sector from disk.
;
;	ax	linear address
;	es:bx	buffer
;
; Returns:	CF set on error
;

read1sector:
	pusha
	push	es
	
	call	linconv			; calculate 3d address

	mov	al,1			; 1 sector to read
	int	0x13			; read sector

	pop	es
	popa

	ret

deb:
	pushad
	xor	ebx,ebx
	mov	eax,0xe * 256 + "X"
	int	0x10
	popad
	ret

;- DATA -----------------------------------------------

msg_a20:
db	"A20 line",0

msg_mem:
db	"Extended memory",0

msg_kernel_read_map:
db	"Kernel map",0

msg_MB:						; is for megabyte
db	"MB",NL,0

msg_ok:
db	"OK",NL,0

msg_error:
db	"ERROR",NL,0

msg_unknown:
db	"?",NL,0

system_memory_map:				; 42-byte system memory map for int 0xc7
map_start:					; also kernel load map will be placed here later

times	2048-512-($-$$) db 0
