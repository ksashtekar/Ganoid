#include <ganoid/types.h>
#include <kernel.h>
#include <isr.h>
#include <idt.h>
#include <common.h>
#include <kdebug.h>

#define MAX_ISR_NUMBER 32+16
static isr_handler_func isr_handlers[MAX_ISR_NUMBER] = {0};
static bool isr_dispatcher_initialized = false;
void isr_handler(isr_registers_t isr_registers);

void init_isr_dispatcher (void)
{
	for (int i = 0; i < MAX_ISR_NUMBER;i++)
		isr_handlers[i] = 0;

	isr_dispatcher_initialized = true;
}


void isr_handler(isr_registers_t isr_registers)
{
	isr_handler_func handler;
	//printk ("interrupt received %d \n", isr_registers.int_no);


	if ( (isr_registers.int_no >= 32) &&
	     (isr_registers.int_no <= 47)){
		// int comes from apic
		//printk ("comes from APIC\n");

		if (isr_registers.int_no >= 40){
			// comes from slave
			outb (0xA0, 0x20);
		      }
		// now to master
		outb (0x20, 0x20);
	}
	if (isr_handlers[isr_registers.int_no]){
		//printk ("IRQ_%d\n", isr_registers.int_no);
		handler = isr_handlers[isr_registers.int_no];
		(*handler)();
	}
}

void add_isr_handler (int irq_no, isr_handler_func isr)
{
	_ASSERT_DEBUG(isr_dispatcher_initialized,
		      EDispatcherNotInitialized,
		      0,0,0);

	_ASSERT(isr, ENullPointer, (unsigned)isr, 0, 0);

	isr_handlers[irq_no] = isr;
}




