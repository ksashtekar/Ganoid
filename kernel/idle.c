/*
 *  kernel/idle.c
 *
 *  Idle thread
 *
 *  Copyright (C) 2011-2022 Kaustubh Ashtekar
 *
 */


#include <kernel/idle.h>


void do_idle()
{
    //schedule_request = 0x10;
    while (1) {
	//ENTER_CRITICAL_SECTION;
	//printf("IDLE TASK\n"); 
	//EXIT_CRITICAL_SECTION;
    }
}
