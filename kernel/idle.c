/*
 *  kernel/idle.c
 *
 *  Idle thread
 *
 *  Copyright (C) 2011-2022 Kaustubh Ashtekar
 *
 */

#include <kernel/idle.h>
#include <task.h>

struct task_struct idle_task_s = {
	.task_name = "idle_task",
	.esp = 0xDEADBEEF,
	.eip = 0xDEADBEEF
};

void do_idle()
{
	/*schedule_request = 0x10; */
	while (1) {
		/*ENTER_CRITICAL_SECTION; */
		printk("IDLE TASK\n");
		/*EXIT_CRITICAL_SECTION; */
	}
}
