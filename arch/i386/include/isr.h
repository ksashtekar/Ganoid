#ifndef ISR_H_
#define ISR_H_

typedef void (*isr_handler_func)(void);

#define TIMER_INTERRUPT 0x20
#define KBD_INTERRUPT 0x21



typedef struct
{
	unsigned gs, fs, es, ds;                  // Data segment selector
	unsigned edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
	unsigned int_no, err_code;    // Interrupt number and error code (if applicable)
	unsigned eip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
}isr_registers_t;


void add_isr_handler (int irq_no, isr_handler_func isr);
void init_isr_dispatcher (void);

#endif
