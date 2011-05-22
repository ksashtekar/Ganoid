/*
 *  arch/i386/kernel/utils.h
 *   
 *  Header file for the utils.S function. 
 *
 *  Copyright (C) 2010-2020  Kaustubh Ashtekar
 *
 *  2010-02-12  Initial version
 */


// enable the interrupts of the processor
void enable_interrupts (void);

// disable the interrupts and return the current EFLAGS register
unsigned int disable_interrupts (void);

// restore the interrupts status
int restore_interrupts (unsigned int eflags);


