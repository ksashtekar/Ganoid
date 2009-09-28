#include <ktype.h>
#include <kernel.h>
#include <isr.h>
#include <idt.h>
#include <common.h>

isr_handler_func isr_handlers[32+16];


void isr_handler (struct isr_registers isr_registers)
{
	isr_handler_func handler;
	//printf ("interrupt received %d \n", isr_registers.int_no);


	if ( (isr_registers.int_no >= 32) &&
	     (isr_registers.int_no <= 47)){
		// int comes from apic
		//printf ("comes from APIC\n");

		if (isr_registers.int_no >= 40){
			// comes from slave
			outb (0xA0, 0x20);
		      }
		// now to master
		outb (0x20, 0x20);
	}
	if (isr_handlers[isr_registers.int_no]){
		printf ("IRQ_%d\n", isr_registers.int_no);
		handler = isr_handlers[isr_registers.int_no];
		(*handler)();
	}
}




void add_isr_handler (int irq_no, isr_handler_func isr)
{
	isr_handlers[irq_no] = isr;
}




