/*
 *  arch/i386/kernel/utils.S
 *   
 *  Utility functions.
 *
 *  Copyright (C) 2010-2020  Kaustubh Ashtekar
 *
 *  2010-02-12  Initial version
 */

#define ASM 1	
	
	.text	
	.code32


	.global enable_interrupts
	.global	disable_interrupts
	.global restore_interrupts

	
enable_interrupts:	
	sti
	ret

disable_interrupts:	
	pushf
	cli
	pop %eax
	ret


	
restore_interrupts: // (unsigned int eflags)
	mov 	4(%esp), %eax
	
	push	%eax
	popf
	
	ret
	

