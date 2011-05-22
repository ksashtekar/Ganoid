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
#include <kbd.h>
#include <task.h>
#include <sched.h>
#include <process.h>
#include <kbd-handler.h>
#include <kdebug.h>
#include <utils.h>

int kernel_main (void);
void my_rtc_irq_handler (void);
void my_page_fault_handler (void);
//void my_kbd_interrupt_handler (void);

extern int multiboot_struct_ptr;

char *process_name="Hello";

struct task_struct idle_task_s = {
    .task_name = "idle_task",
    .esp = 0xDEADBEEF,
    .eip = 0xDEADBEEF
};

extern struct task_struct *current_task;
extern int schedule_request;

int re (void);


int process_1 (void *p)
{
    u32 *frame_ptr;
#if 0
    ENTER_CRITICAL_SECTION;
    asm volatile ("mov %%ebp, %0\n" : : "m"(frame_ptr));
    printf("Frame pointer: 0x%x\n", frame_ptr);
    printf("0x%x: 0x%x\n", frame_ptr + 3, *(frame_ptr+3));
    printf("0x%x: 0x%x\n", frame_ptr + 2, *(frame_ptr+2));
    printf("0x%x: 0x%x\n", frame_ptr + 1, *(frame_ptr+1));
    printf("0x%x: 0x%x\n", frame_ptr + 0, *(frame_ptr+0));
    while(1);
#endif // 0
    int j = 0;
    //while(1) {
	//ENTER_CRITICAL_SECTION;
	printf ("process 1: %d : %s\n", j++, p);
	//EXIT_CRITICAL_SECTION;
	//}
    return 0;
}
    
int process_2 (void *p)
{
    while(1) {
	ENTER_CRITICAL_SECTION;
	printf ("process 2: %s\n", p);
	EXIT_CRITICAL_SECTION;
    }
    return 0;
}


int process_3 (void *p)
{
    while(1) {
	ENTER_CRITICAL_SECTION;
	printf ("process 3: %s\n", p);
	EXIT_CRITICAL_SECTION;
    }
    return 0;
}



int kernel_main ()
{
    int r = 0;
    unsigned char val;

    delay (0);
    //delay (0);
    //delay (0);
    //delay (0);
    //delay (0);


    vga_clearscreen ();


    read_multiboot_information ((u32*)multiboot_struct_ptr);
    display_boot_progress ("Read multiboot information", 0);
    r  = init_bootmem_allocator ();
    display_boot_progress ("Initialize bootmem allocator", r);

    init_isr_dispatcher ();
    init_Interrupt_Descriptor_Table ();
    r = init_APIC ();
    display_boot_progress ("Initialize interrupt subsystem", r);

    r = init_kbd_handler ();
    display_boot_progress ("Initialize keyboard", r);

    init_timer (HZ);
    add_isr_handler (TIMER_INTERRUPT, timer_isr);

    //add_isr_handler (14, my_page_fault_handler);

    //asm volatile ("sti");

    /*
      init_paging ();
      printf ("i am now working in paging mode\n");
      int i = 10;
      p = (unsigned*)&i;
      p = (unsigned*)0x10000000;
      (*p) = 0;
      i++;
	
      // asm volatile ("int $");

      asm volatile("mov $0xBADBADBA, %edx");
    */

    //    asm volatile ("sti");

    init_schedular();
    kernel_thread(process_1, "Kaustubh", 0);
    kernel_thread(process_2, "Smita", 0);
    kernel_thread(process_3, "Tejas", 0);

    asm volatile ("sti");	
    idle_task();
    while (1){}
    //kernel_thread (process_2, NULL, 0); 

    // call idle thread



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

    int ui = 0x100;
    re ();
    asm ("mov %%eax, %0"::"m"(ui));
    printf ("ui = 0x%x\n", ui);
    while (1){}

    return 0;
}

int re (void)
{
    return 0xBAD;
}

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
	printf ("%2d:%2d:%2d\n", hr, min, sec);
    }
    i++;
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

void idle_task (void)
{
    //schedule_request = 0x10;
    while (1) {
	ENTER_CRITICAL_SECTION;
	printf("IDLE TASK\n"); 
	EXIT_CRITICAL_SECTION;
    }
}




