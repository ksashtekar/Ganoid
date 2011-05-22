#ifndef _SCHED_H_
#define _SCHED_H_

#include <task.h>

void HZ_Isr (void);
int init_schedular (void);
int add_task_to_run_queue (struct task_struct *task);
void print_all_tasks (void);


#endif // _SCHED_H_
