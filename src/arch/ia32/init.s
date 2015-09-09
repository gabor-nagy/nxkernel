[CPU 386]
[BITS 32]

[global start]

[extern _main]
[extern _total_memory]

start:
	cmp	eax,0x2BADB002		; see whether multiboot is supported
	jne	.not_multiboot
	mov	eax,dword [ebx]
	test	eax,1			; if so, see if memory info is given
	jz	.no_memory_info
	mov	eax,dword [ebx+8]
	add	eax,1024
	mov	[_total_memory],eax
	jmp	.multiboot_ok

.not_multiboot:
.no_memory_info:
        mov	eax,0x1f * 256 + '?'
	mov	ebx, 0xb8000
	mov	[ebx], eax		; display '?' in the top-left corner to indicate error
	jmp	.halt

.multiboot_ok:
	call	.get_eip
.get_eip:
	pop	esp			; create kernel stack (0 -> stack top)

	call	_main			; call high-level code

.halt:
	cli
	hlt
	jmp 	$			; halt
