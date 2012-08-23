#ifndef _TASK_H
#define _TASK_H

#include <isr.h>
#include <mm.h>

/*
  struct processor_context 
  {
  unsigned cs;
  unsigned ds;
  unsigned ss;
  unsigned es;
  unsigned fs;
  unsigned gs;

  unsigned eax;
  unsigned ebx;
  unsigned ecx;
  unsigned edx;
  unsigned esi;
  unsigned edi;
  unsigned esp;
  unsigned ebp;
  unsigned eip;
  };
*/

#define DEFAULT_STACK_SIZE PAGE_SIZE
#define DEFAULT_PROCESS_PRIORITY 0

enum {
	THREAD_RUNNING,
	THREAD_RUNNABLE,
	THREAD_BLOCKED
};

struct task_struct {
	char task_name[64];

	/* Process context */
	/*
	 * The stack, apart from normal use, is also used for storing the
	 * state of the processor registers during a context switch.
	 */
	unsigned char stack[DEFAULT_STACK_SIZE];

	unsigned esp;
	unsigned eip;
	unsigned isr_ebp;
	
	unsigned priority;
	unsigned pid; /* Process ID */
	unsigned state; /* Blocked/Runnable */

	struct task_struct *prev, *next;
};

#endif
