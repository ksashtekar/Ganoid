/*
 *  arch/i386/kernel/cpu-id.S
 *
 *  Read and store the complete cpu information obtained
 *  from the cpuid instruction.
 *
 *  Copyright (C) 2009-2020  Kaustubh Ashtekar
 *
 *  20XX-XX-XX  <put any credits for changes here along with
 *               names>
 */

#include <cpu.h>	

	.code32
	.text
	.global check_if_right_cpu


check_if_right_cpu:
	// first check if cpuid inst is supported
	pushf
	pop	%eax
	// set bit 21.
	or	$0x200000, %eax
	push	%eax
	popf
	pushf
	pop 	%eax
	and	$0x200000, %eax
	jz	cpuid_not_supported

	// load the buffer address
	lea	raw_cpu_data_buffer, %edi

	// now start reading information
	mov	$0, %eax
	cpuid

	// save max basic function no.
	mov	%eax, max_basic_funct_no
	mov	%ebx, (%edi)
	add	$4, %edi
	mov	%ecx, (%edi)
	add	$4, %edi
	mov	%edx, (%edi)
	add	$4, %edi

	mov 	$0x01, %eax
	cmp	max_basic_funct_no, %eax
	jg	max_basic_reached	
	cpuid
	mov	%eax, (%edi)
	add	$4, %edi
	mov	%ebx, (%edi)
	add	$4, %edi
	mov	%ecx, (%edi)
	add	$4, %edi
	mov	%edx, (%edi)
	add	$4, %edi

	mov	$0x02, %eax
	cmp	max_basic_funct_no, %eax
	jg	max_basic_reached	
	cpuid
	mov	%eax, (%edi)
	add	$4, %edi
	mov	%ebx, (%edi)
	add	$4, %edi
	mov	%ecx, (%edi)
	add	$4, %edi
	mov	%edx, (%edi)
	add	$4, %edi
	
max_basic_reached:

	mov 	$0x00, %eax
	ret
	
cpuid_not_supported:
	mov	$0x01, %eax
	ret