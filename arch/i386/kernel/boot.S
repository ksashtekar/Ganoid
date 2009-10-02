   /* ----------------------------------------------------------------------- *
    *
    *   Copyright (C) 2009, 2010 Kaustubh Ashtekar
    *
    *   This file is part of the Ufeox kernel, and is made available under
    *   the terms of the GNU General Public License version 2.
    *   This file is adopted from example file in the grub doc.
    *
    *   File: boot.S
    *   Description: This file is the first contains the code that executes
    *                immediately after the bootloader.
    *                This kernel follows the Multiboot specification. This
    *	file contains the Multiboot header required for booting this kernel.	
    *
    * ----------------------------------------------------------------------- */

	

#define ASM 1
	
#include <multiboot.h>


	
	.text
	.globl start, _start

start:
_start:
	jmp multiboot_entry

	.align 4
	/* Multiboot header starts here*/
	/*
	The layout of the Multiboot header must be as follows:

	Offset 	Type 	Field Name 	Note
	0 	u32 	magic 	required
	4 	u32 	flags 	required
	8 	u32 	checksum 	required
	12 	u32 	header_addr 	if flags[16] is set
	16 	u32 	load_addr 	if flags[16] is set
	20 	u32 	load_end_addr 	if flags[16] is set
	24 	u32 	bss_end_addr 	if flags[16] is set
	28 	u32 	entry_addr 	if flags[16] is set
	32 	u32 	mode_type 	if flags[2] is set
	36 	u32 	width 	if flags[2] is set
	40 	u32 	height 	if flags[2] is set
	44 	u32 	depth 	if flags[2] is set
	*/
	.long MULTIBOOT_HEADER_MAGIC
	.long MULTIBOOT_HEADER_FLAGS
	.long -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
// we will be using ELF file ... so no need of the additional fields

multiboot_entry:
	cli
	// init the stack pointer
	movl $(stack + STACK_SIZE), %esp

	// reset the EFLAGS register
	pushl $0
	popf

	// push the pointer to the multiboot information structure
	pushl %ebx
	// push the magic value
	pushl %eax

	// bss space has been init'ed by the good bootloader	
	
	// now enter the C main function
	call EXT_C(cmain)

	// halt
	//pushl $halt_message
	//call EXT_C(printf)
loop:
	cli
	hlt
	jmp loop

halt_message:
	.asciz "Halted."


	// stack area
	.comm stack, STACK_SIZE
	
	
	
