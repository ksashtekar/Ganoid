/*
 *  kernel/panic.c
 *
 *  Kernel panic handler.
 *
 *  Copyright (C) 2011-2022 Kaustubh Ashtekar
 *
 */


#include <kernel.h>


void panic()
{
	printk("PANIC\n");
	ENTER_CRITICAL_SECTION;
	while(1);
}
