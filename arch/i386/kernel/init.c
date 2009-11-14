#include <ktypes.h>
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
#include <paging.h>
#include <sysinfo.h>
#include <init.h>
#include <bootmem.h>

int cmain (u32 magic_val, u32 *multiboot_info);
void my_rtc_irq_handler (void);
void my_page_fault_handler (void);



//int i = 10;

int cmain (u32 magic_val, u32 *multiboot_info)
{
	int r = 0;
	unsigned char val;
	uint *p;

	if (magic_val != 0x2BADB002)
		while (1);
	


	vga_clearscreen ();
	read_multiboot_information (multiboot_info);
	display_boot_progress ("Read multiboot information", 1);
	r  = init_bootmem_allocator ();
	display_boot_progress ("Initialize bootmem allocator", r);
	int *p1; // = (int*)bm_malloc (16777216);
	int ii = 3;
	while (ii--) {
		p1 = (int*)bm_malloc (4096); //16777216);
	//if (p1) printf ("allocation success\n");
	//else printf ("allocation fail\n");
	printf ("Address ::0x%8x\n", (uint)p1);
	}
	while (1);


	//printf ("GDT init in progress !\n");
	init_GDT ();
	printf ("IDT init in progress !\n");
	init_Interrupt_Descriptor_Table ();
	add_isr_handler (0x20, my_rtc_irq_handler);
	add_isr_handler (14, my_page_fault_handler);

	init_APIC ();
	//init_timer (5);
	asm volatile ("sti");
	init_paging ();
	printf ("i am now working in paging mode\n");
	int i = 10;
	p = (unsigned*)&i;
	p = (unsigned*)0x10000000;
	(*p) = 0;
	i++;
	
	// asm volatile ("int $");

	asm volatile("mov $0xBADBADBA, %edx");
	while (1){
		//asm volatile ("hlt");
		//asm volatile ("int $14");
	}

	//char c = '0';

	//  int i = 10;
	// while(1)
 
	//ExecuteTests (0);
	while (1);

	asm volatile ("xor %ebx, %ebx");
	asm volatile ("add $100, %ebx");
	asm volatile ("add $0, %0": "=a"(val): "a"(val));

	return 0;
}



void my_rtc_irq_handler (void)
{
	static int i= 0;
	printf ("OS tick count: %d\n", i++);
}

void my_page_fault_handler (void)
{
	printf ("Page_Fault_Occured !");
	// interrupts are disabled here automatically b'coz we are in isr
	while (1);
}


void delay (unsigned long n)
{
	if (!n)
		n = 2000000;
	while(n--){}
}


void irq0_handler (void);
void irq0_handler (void)
{
	printf ("into irq0 handler");
}


