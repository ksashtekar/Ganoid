/*
 *  kernel/idle.c
 *
 *  Page fault handler
 *
 *  Copyright (C) 2011-2022 Kaustubh Ashtekar
 *
 */


#if 0


void my_page_fault_handler (void);



void my_page_fault_handler (void)
{
    printf ("Page_Fault_Occured !");
    // interrupts are disabled here automatically b'coz we are in isr
    while (1);
}



int init_mm()
{
    //add_isr_handler (14, my_page_fault_handler);    
}
#endif // 0
