#ifndef _PROCESS_H_
#define _PROCESS_H_

int kernel_thread(int (*fn)(void *), void * arg, unsigned long flags);

#endif // _PROCESS_H_
