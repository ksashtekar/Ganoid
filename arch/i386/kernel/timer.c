#include <common.h>
#include <vga.h>
#include <tests.h>
#include <gdt.h>
#include <kernel.h>
#include <string.h>
#include <idt.h>
#include <isr.h>
#include <apic.h>
#include <timer.h>

int temp1, temp2;

// assuming the timer mode 
void print_timer0_val (void)
{
	unsigned int timer_val = 0;
	unsigned int temp = 0;
	// first ask the 8253 to store the count in its internal latch
	outb (0x43, 0x00);
 
	// now read the value lsb first
	timer_val = inb(0x40); // LSB
	temp = inb (0x40); // MSB

	timer_val = (timer_val | (temp <<8));

	printf ("%d\n", timer_val);
}


void init_timer(unsigned int frequency)
{
   // Firstly, register our timer callback.
   //register_interrupt_handler(IRQ0, &timer_callback);
	
   // The value we send to the PIT is the value to divide it's input clock
   // (1193180 Hz) by, to get our required frequency. Important to note is
   // that the divisor must be small enough to fit into 16-bits.
   unsigned int divisor = 1193180 / frequency;

   // Send the command byte.
   outb(0x43, 0x34);

   // Divisor has to be sent byte-wise, so split here into upper/lower bytes.
   unsigned char l = (unsigned char)(divisor & 0xFF);
   unsigned char h = (unsigned char)( (divisor>>8) & 0xFF );

   // Send the frequency divisor.
   outb(0x40, l);
   outb(0x40, h);

   /*
   while (1){
	   // print_timer0_val ();
   read_int_request_reg (&temp1, &temp2); 
   read_in_service_reg (&temp1, &temp2);
   delay (12000000);
   }*/
} 
