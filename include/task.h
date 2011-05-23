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

typedef struct
{
    char task_name[64];
    unsigned char stack[DEFAULT_STACK_SIZE];

    unsigned esp;
    unsigned eip;
    unsigned isr_ebp; 

    struct task_struct *prev;
    struct task_struct *next;
}task_struct_t;






#endif
