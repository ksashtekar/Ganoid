/*
 *  arch/i386/kernel/cpu.c
 *
 *  Parse raw data read by cpu-id.S.
 *
 *  Copyright (C) 2009-2020  Kaustubh Ashtekar
 *
 *  20XX-XX-XX  <put any credits for changes here along with
 *               names>
 */

#include <cpu.h>
#include <kernel.h>

/* ****************** filled by cpu-id.S ******************* */
int max_basic_funct_no;
int max_ext_funct_no;
unsigned char raw_cpu_data_buffer[RAW_CPU_DATA_BUFFER_SIZE] = { 0 };

/* ********************************************************* */

void print_cpuid_info()
{
	for (int i = 0; i < 16; i++) {
		printk("%x: %c\n", raw_cpu_data_buffer[i],
		       raw_cpu_data_buffer[i]);
	}
}
