#include <ganoid/types.h>
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
#include <kernel/idle.h>

int kernel_main (void);


extern int multiboot_struct_ptr;
extern task_struct_t *current_task;
extern int schedule_request;

  

int kernel_main ()
{
    int r = 0;

    delay (0);

    vga_init ();

    read_multiboot_information ((u32*)multiboot_struct_ptr);
    display_boot_progress ("Read multiboot information", 0);
    //r = init_free_page_data();
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

    //asm volatile ("sti");

    /*
      init_paging ();
      printk ("i am now working in paging mode\n");
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
    asm volatile ("sti");	
    do_idle();

#if 0
    unsigned char val;
    asm volatile ("xor %ebx, %ebx");
    asm volatile ("add $100, %ebx");
    asm volatile ("add $0, %0": "=a"(val): "a"(val));
#endif // 0

    return 0;
}




void irq0_handler (void);
void irq0_handler (void)
{
    printk ("into irq0 handler");
}
