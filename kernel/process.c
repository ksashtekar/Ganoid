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
#include <kbd-handler.h>
#include <task.h>
#include <sched.h>
#include <kdebug.h>



// for now only have a static array of empty structures of task_struct. will
// use dynamic allocation in the future
task_struct_t task_struct_array[10] __attribute__((aligned(4)));
static int task_struct_array_index = 0;


static task_struct_t* get_free_task_struct (void);


static void hello()
{
    while(1){
	ENTER_CRITICAL_SECTION;
	printf("Process exited\n");
	EXIT_CRITICAL_SECTION;
    }
}


/*
 * Create a new kernel thread. 
 * fn: Function which will be called after the new thread is created.
 */
int kernel_thread(int (*fn)(void *), void * arg, unsigned long flags)
{
    printf("Start kernel thread creation\n");
    task_struct_t *t = get_free_task_struct ();
    printf("Task struct allocated at: 0x%x\n", t);
    t->task_name[0] = 'X';
    t->task_name[1] = '\0';

    // map the isr_registers struct to the stack space. this will help us when 
    // this process needs to be invoked first time.
    unsigned* sptr = t->stack + DEFAULT_STACK_SIZE - 4;
    *sptr = arg; // argument for first function in the new thread
    sptr--;
    *sptr = hello; // dummy EIP ... this would be the place where 'fn'
    printf("%s: 0x%x\n", arg, sptr);
    // would return ... FIXME: Fill this later with OS exit
    // function
    const char *func_sptr = sptr;
    //sptr--;
    //*sptr = 
    isr_registers_t* i = (sptr - (sizeof(isr_registers_t)/4));


    // FIXME: This may be required for interrupt from user space
    //    i->ss = 0x10;
    //i->useresp = func_sptr;
    i->eflags = 0x00200260;
    //i->eflags = 0x00000400; FIXME: proper value needed
    i->cs = 0x08; // code segment
    i->eip = fn;

    i->int_no = 0;
    i->err_code = 0;    // Interrupt number and error code (if applicable)

    i->eax = 0;
    i->ecx = 0;
    i->edx = 0;
    i->ebx = 0;
    i->esp = sptr;
    i->ebp = 0;
    i->esi = 0;
    i->edi = 0;

    i->ds = 0x10; // copied blindly from kernel.S. needs to change when
    // proper implementation is made.
    i->es = 0x10;
    i->fs = 0x10;
    i->gs = 0x10;

    t->esp = i;

    //printf("Add just created thread to active list...\n");
    add_task_to_run_queue(t);
}



// for now this function is made compatible with the static task struct array.
// this will change once we implement dynamic allocation for these structures.
static task_struct_t* get_free_task_struct ()
{
    int allocated_index = 0;
    ENTER_CRITICAL_SECTION;
    allocated_index = task_struct_array_index++;
    EXIT_CRITICAL_SECTION;
    return &task_struct_array[allocated_index];
}
