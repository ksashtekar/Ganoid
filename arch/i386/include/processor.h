#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__

// enable the interrupts of the processor
void enable_interrupts (void);

// disable the interrupts and return the current EFLAGS register
unsigned int disable_interrupts (void);

// restore the interrupts status
int restore_interrupts (unsigned int eflags);


#endif /* __PROCESSOR_H__ */
