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
#include <kbd-handler.h>
#include <task.h>
#include <sched.h>
#include <kdebug.h>
#include <utils.h>


extern task_struct_t idle_task_s;

// this is the head of the run queue.
static task_struct_t *head = NULL;

task_struct_t *current_task = NULL;

int schedule_request = 0x10;

u32 schedule_pending(isr_registers_t isr_reg); // __attribute__((naked));

static void add_idle_task_to_run_queue(task_struct_t *task)
{
    if(head != NULL) {
	printf("adding idle task twice ?\n");
	while(1){}
    }

    head = task;
    task->prev = NULL;  
    task->next = NULL;
    current_task = &idle_task_s;
}


static task_struct_t* get_next_task (task_struct_t *cur_task)
{
    task_struct_t *next = cur_task->next;
    //printf("Return: %s\n", next->task_name);
    if (!next) 
	return head;
    else 
	return next;
}

/*
 * Following are the assumptions when this function will be called.
 * 1. The processor context is saved in the following order on the stack.
 *    EFLAGS, CS, EIP, EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI
 * 
 * The stack is the same one of the process which was interrupted.
 */
u32 schedule_pending (isr_registers_t isr_reg)
{
    schedule_request = 0;
    //printf("ENTER SCH\n");
	
    u32 *frame_ptr;
    asm volatile ("mov %%ebp, %0\n" : : "m"(frame_ptr));
    //printf("ESP_B: 0x%x\n", frame_ptr);
    frame_ptr += 2; // skip over EIP and saved EBP (by this function)
    current_task->esp = frame_ptr;
    //printf("ESP_A: 0x%x\n", frame_ptr);


    // save current process context
    //u32 ebp_;
    //	asm volatile("pop %eax");
    //asm volatile("mov %%eax, %0\n" : : "m"
    //     (current_task->isr_ebp));
	

    //asm volatile ("mov %%esp, %0\n" : : "m"(esp_));
    //asm volatile ("mov %%esp, %0\n" : : "m"(current_task->esp));
    //current_task->esp = esp_;
    current_task = get_next_task(current_task);

    // test
    //	unsigned char* sptr = current_task->stack;
    //struct isr_registers* i = (sptr + PAGE_SIZE - 
    //sizeof(struct isr_registers));
    // ~test

    //printf("Run: %s III---III\n", current_task->task_name);
    //ebp_ = current_task->esp;
    //asm volatile ("mov %0, %%esp\n" : : "m"(esp_));
    //asm volatile ("mov %0, %%esp\n" : : "m"(current_task->esp));


    //	asm volatile("mov %0, %%eax\n" : : "m"
    //     (current_task->isr_ebp));
    //asm volatile("push %eax");



    //printf("EXIT SCH\n");
    return current_task->esp;
}


int init_schedular ()
{
    head = NULL;
    add_idle_task_to_run_queue (&idle_task_s);
    return 0;
}


int add_task_to_run_queue (task_struct_t *task)
{
    task_struct_t *node = NULL;
    printf("Adding %s to run queue ...\n", task->task_name);

    if (head == NULL) {
	printf ("Adding other tasks before adding idle task?\n");
	while (1){}
    }

    ENTER_CRITICAL_SECTION;
    // go till the end of the list ...
    for (node = head; node->next!=NULL; node = node->next) {}

    node->next = task;
    task->prev = node;
    task->next = NULL;
    EXIT_CRITICAL_SECTION;
    return 0;
}



void print_all_tasks ()
{
    task_struct_t *node = NULL;

    ENTER_CRITICAL_SECTION;

    printf ("List of all threads...\n");
    int i = 1;
    // go till the end of the list ...
    for (node = head; node!=NULL; node = node->next, i++) {
	printf ("%d: %s\n", i, node->task_name);
    }

    EXIT_CRITICAL_SECTION;
}
