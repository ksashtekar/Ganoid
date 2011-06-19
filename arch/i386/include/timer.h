#ifndef TIMER_H_
#define TIMER_H_

void delay (unsigned long n);

void init_timer(unsigned int frequency);
void print_timer0_val (void);

void timer_isr (void);


#endif
