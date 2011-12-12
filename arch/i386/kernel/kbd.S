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

	.data
	
kbd_status:	
	.byte 0x00


	.text
	.code32

	.global init_kbd
	.global kbddelay1
	.global read_byte_from_dat_buf
	.global disable_keyboard_int
	.global enable_keyboard_int

	.set status_port,   0x64
	.set kbd_out_buf,   0x60
	.set kbd_cmd_buf,   0x64
	.set kbd_data_buf,  0x60
	.set port_a, 	    0x60
	.set port_b,        0x61
	.set out_buf_full,  0x01
	.set inpt_buf_full, 0x02
	.set sys_flag,	    0x04
	.set cmd_data,      0x08
	.set keybd_inh,     0x10
	.set trans_tmout,   0x20
	.set rcv_tmout,     0x40
	.set parity_even,   0x7f
	.set inh_keyboard,  0x10
	.set kbd_ena,       0xae
	.set kbd_dis,       0xad
				// 8042 commands
	.set k_8042_read_cmd, 0x20
	.set k_8042_write_cmd, 0x60
	.set k_8042_self_test, 0xAA
				// 8042 response
	.set k_8042_self_test_res, 0x55
				// keyboard commands
	.set kb_menu,       0xf1		
	.set kb_enable,     0xf4
	.set kb_makebreak,  0xf7
	.set kb_echo,       0xfe
	.set kb_reset,      0xff
	.set kb_led_cmd,    0xed
	.set kb_scan_code,  0xf0
	.set kb_ret_scan,   0x00
	.set kb_scan_1,     0x01
	.set kb_scan_2,     0x02
	.set kb_scan_3,     0x03
	
				// keyboard responses
	.set kb_ok,         0xaa
	.set kb_ack,        0xfa 
	.set kb_overrun,    0xff
	.set kb_resend,     0xfe
	.set kb_break,      0xf0
	.set kb_fa,         0x10
	.set kb_fe,         0x20
	.set kb_pr_led,     0x40
	


init_kbd:
	// assume nobody has initialized the keyboard yet
	// disable interrupt from the 8042
	// read 8042 command byte

	push	%ebx

	call wait_for_ip_buf_free
	mov $k_8042_read_cmd, %al
	outb %al, $kbd_cmd_buf
	call wait_for_data_in_output_buf
	

	inb $kbd_data_buf, %al
	// al = 8042 cmd register
	
	//// clear the interrupt bit
	and $~0x01, %al
	// set the interrupt bit
	//or $0x01, %al
	mov %al, %bl

	call wait_for_ip_buf_free
	mov $k_8042_write_cmd, %al
	outb %al, $kbd_cmd_buf
	call wait_for_ip_buf_free
	//call kbddelay
	mov  %bl, %al
	outb %al, $kbd_data_buf
	
	call wait_for_ip_buf_free
	mov $k_8042_self_test, %al
	outb %al, $kbd_cmd_buf
	call wait_for_data_in_output_buf

	inb $kbd_data_buf, %al
	cmp $k_8042_self_test_res, %al
	jne init_kbd_error

	// momentarily lit on all the LED's
	mov	$kb_led_cmd, %al
	mov 	$0x07, %bl
	call	send_command_and_data_to_kbd_wait_ack
	//call	kbddelay1


//2:
	//jmp	2b

	// off all the led's	
	mov 	$kb_led_cmd, %al
	mov 	$0x00, %bl
	call	send_command_and_data_to_kbd_wait_ack
	//call	kbddelay1

	// set internal status to zero since all LED's are zero
	xor	%al, %al
	mov 	%al, kbd_status

	// add this when working on actual PC
	call	select_scan_code_2

	// now enable interrupts -----------------
	call wait_for_ip_buf_free
	mov $k_8042_read_cmd, %al
	outb %al, $kbd_cmd_buf
	call wait_for_data_in_output_buf

	inb $kbd_data_buf, %al
	// al = 8042 cmd register
	// set the interrupt bit
	or $0x01, %al
	mov %al, %bl
	
	call wait_for_ip_buf_free
	mov $k_8042_write_cmd, %al
	outb %al, $kbd_cmd_buf
	call wait_for_ip_buf_free
	mov  %bl, %al
	outb %al, $kbd_data_buf
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	movl	$0, %eax
	pop	%ebx
	ret

read_byte_from_dat_buf:
	call 	wait_for_data_in_output_buf
	inb	$kbd_data_buf, %al
	ret


select_scan_code_2:
	push 	%ebx
	push 	%ecx
	mov	$kb_scan_code, %al
	mov     $kb_scan_2, %bl
	call    send_command_and_data_to_kbd_wait_ack
	pop	%ecx
	pop	%ebx
	ret


	// --------------------
wait_for_ip_buf_free:
	push %ecx
	push %eax
	sub %cx, %cx
1:	
	inb $status_port, %al
	test $inpt_buf_full, %al
	loopnz 1b
	pop %eax
	pop %ecx
	ret
	// --------------------


	// ---------------------
	// al = command
	// bl = data
send_command_and_data_to_kbd_wait_ack:
	call	wait_for_ip_buf_free
	outb 	%al, $kbd_data_buf
	call 	wait_for_data_in_output_buf
	inb  	$kbd_data_buf, %al
	cmp 	$kb_ack, %al
	jne     init_kbd_error
	mov 	%bl, %al
	outb 	%al, $kbd_data_buf
	call	wait_for_data_in_output_buf
	inb	$kbd_data_buf, %al
	cmp	$kb_ack, %al
	jne	init_kbd_error
	ret
	
	

	//---------------------
wait_for_data_in_output_buf:
	push %ecx
	push %eax
	sub %cx, %cx
1:
	inb $status_port, %al
	test $out_buf_full, %al
	loopz 1b
	pop %eax
	pop %ecx
	ret


disable_keyboard_int:	
	push	%eax
	push	%ebx
	
	call	wait_for_ip_buf_free
	mov	$k_8042_read_cmd, %al
	outb	%al, $kbd_cmd_buf
	call	wait_for_data_in_output_buf
	inb	$kbd_data_buf, %al
	// al = 8042 cmd register
	
	// clear the interrupt bit
	and	$~0x01, %al
	mov	%al, %bl

	call 	wait_for_ip_buf_free
	mov 	$k_8042_write_cmd, %al
	outb 	%al, $kbd_cmd_buf
	call 	wait_for_ip_buf_free
	mov  	%bl, %al
	outb 	%al, $kbd_data_buf

	
	pop	%ebx
	pop	%eax
	ret

enable_keyboard_int:	
	push	%eax
	push	%ebx
	
	call	wait_for_ip_buf_free
	mov	$k_8042_read_cmd, %al
	outb	%al, $kbd_cmd_buf
	call	wait_for_data_in_output_buf
	inb	$kbd_data_buf, %al
	// al = 8042 cmd register
	
	// clear the interrupt bit
	or	$0x01, %al
	mov	%al, %bl

	call 	wait_for_ip_buf_free
	mov 	$k_8042_write_cmd, %al
	outb 	%al, $kbd_cmd_buf
	call 	wait_for_ip_buf_free
	mov  	%bl, %al
	outb 	%al, $kbd_data_buf

	pop	%ebx
	pop	%eax
	ret	

init_kbd_error:
	mov $0xF1, %eax
	ret
	

kbddelay: 
	push %eax
	mov  $0xFFFFF, %eax
1:
	dec %eax
	cmp $0x0, %eax
	jne  1b
	
	pop %eax
	ret
	
	    
kbddelay1:
	push %eax
	mov $0x3FFFFFF, %ecx
1:
	loopz 1b
	pop %eax
	ret