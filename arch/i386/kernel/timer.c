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

extern int schedule_request;

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

	printk ("%d\n", timer_val);
}


/*
 * We will initialize the 8254 chip here.
 */
void init_timer(unsigned int frequency)
{
   // The value we send to the PIT is the value to divide its input clock
   // (1193180 Hz) by, to get our required frequency. Important to note is
   // that the divisor must be small enough to fit into 16-bits.
   unsigned int divisor = 1193180 / frequency;
	// unsigned int divisor = 1193180 / 10;

   // Send the command byte.
   outb(0x43, 0x36);
   // outb(0x43, 0x34);

   //   char r = inb (0x43);
   //   int i = r;
   //   printk ("0x%x", i);
   //   while (1){}

   delay (0);

   // Divisor has to be sent byte-wise, so split here into upper/lower bytes.
   unsigned char l = (unsigned char)(divisor & 0xFF);
   unsigned char h = (unsigned char)( (divisor>>8) & 0xFF );

   // Send the frequency divisor.
   outb(0x40, l);
   delay (0);
   outb(0x40, h);
   delay (0);
   /*
   while (1){
	   // print_timer0_val ();
   read_int_request_reg (&temp1, &temp2); 
   read_in_service_reg (&temp1, &temp2);
   delay (12000000);
   }*/
} 



/*
 * This ISR runs at every OS tick. @ the frequency of HZ. 
 */
void timer_isr ()
{
    //printk("TIMER\n");
    schedule_request = 0x10;
}

#if 0
void my_rtc_irq_handler (void)
{
    static int i= 0;
    static int sec  = 0;
    static int min = 45;
    static int hr = 23;
    if (!(i%1000)) {
	sec++;
	if (sec == 60) {
	    min++;
	    sec = 0;
	    if (min == 60){
		min = 0;
		hr++;
		if (hr == 24)
		    hr = 0;
	    }
	}
	printk ("%2d:%2d:%2d\n", hr, min, sec);
    }
    i++;
}
#endif // 0


void delay (unsigned long n)
{
    if (!n)
	n = 2000000;
    while(n--){}
}
