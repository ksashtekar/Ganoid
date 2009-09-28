
#include <kernel-asm.h>

	.file "kernel.S"

	.text
	.code32

	.globl gdt_flush

	// assume that the gdtr pointer is present in the
	// eax register.
gdt_flush:
	lgdt EXT_C(GDTR_val)
	mov $0x10, %ax // segment selector no. 2
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss
	ljmp $0x08, $gdt_flushed_
gdt_flushed_:	
	ret
	
	