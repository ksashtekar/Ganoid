#ifndef ISR_H_
#define ISR_H_

typedef void (*isr_handler_func)(void);


struct isr_registers
{
   unsigned ds;                  // Data segment selector
   unsigned edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
   unsigned int_no, err_code;    // Interrupt number and error code (if applicable)
   unsigned eip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
};


void add_isr_handler (int irq_no, isr_handler_func isr);


#endif
