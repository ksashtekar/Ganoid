	.file "vectors.S"

	.text
	.code32


	.macro ISR  isrnumber
	.global ISR\isrnumber
ISR\isrnumber:
	cli
	push $0
	push $\isrnumber
	jmp isr_common_stub

	.endm



	
	ISR 0
	ISR 1 
	ISR 2 
	ISR 3 
	ISR 4 
	ISR 5 
	ISR 6
	ISR 7 
	ISR 8 
	ISR 9 
	ISR 10 
	ISR 11 
	ISR 12 
	ISR 13 
	ISR 14 
	ISR 15 
	ISR 16
	ISR 17 
	ISR 18 
	ISR 19 
	ISR 20 
	ISR 21 
	ISR 22 
	ISR 23 
	ISR 24 
	ISR 25 
	ISR 26 
	ISR 27 
	ISR 28 
	ISR 29 
	ISR 30 
	ISR 31 
	ISR 32
	ISR 33
	ISR 34
	ISR 35 
	ISR 36 
	ISR 37
	ISR 38
	
	
isr_common_stub:
	pusha
	mov 	%ds, %ax
	push 	%eax
	mov 	%es, %ax
	push	%eax
	mov	%fs, %ax
	push	%eax
	mov	%gs, %ax
	push	%eax
	mov	$0x10, %ax
	mov 	%ax, %ds
	mov 	%ax, %es
	mov 	%ax, %gs
	mov 	%ax, %fs
	call 	isr_handler

	lea	schedule_request, %eax
	cmp	$0,schedule_request
	jz	schedule_not_pending
	call 	schedule_pending
	// EAX contains the new stack pointer
	mov 	%eax, %esp
schedule_not_pending:	
	pop 	%eax 
	mov 	%ax, %gs
	pop	%eax
	mov	%ax, %fs
	pop	%eax
	mov	%ax, %es
	pop	%eax
	mov 	%ax, %ds
	popa
	add $8, %esp // skip the interrupt number and error code
	sti
	iret


/*
check_gbl_access:
	push	%eax
	//lea	r_e, %eax
	movl	$10, r_e
	pop	%eax
	ret
	*/