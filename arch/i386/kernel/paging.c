#include <common.h>
#include <vga.h>
#include <tests.h>
#include <gdt.h>
#include <kernel.h>
#include <string.h>
#include <idt.h>
#include <isr.h>
#include <apic.h>
#include <timer.h>
#include <paging.h>


static unsigned int page_table[1024] __attribute__ ((aligned (0x1000)));
static unsigned int page_directory [1024] __attribute__ ((aligned (0x1000)));

int dummy_variable = 0;



int init_paging (void)
{
    //char teststack [1000];
    unsigned i;
    unsigned cr0;
    memset (page_directory, 0, sizeof (page_directory));
    memset (page_table, 0, sizeof (page_table));


    // we will take some variable in out current address space.
    unsigned page_directory_index = ((unsigned)&dummy_variable & 0xFFC00000)>>22;

    page_directory[page_directory_index] = ((unsigned)page_table & 0xFFFFF000) |
	0x1B;  // disable caching, write through, svc, write, present

    //printk ("%x %x\n", page_directory[page_directory_index], page_table);

    //unsigned page_table_index = ((unsigned)&dummy_variable & 0x003FF000)>>12;
    //unsigned page_table_index = ((unsigned)&init_paging & 0x003FF000)>>12;
    /*
      for (i = 0; i < 10; i++){
      printk ("%x\n", page_directory[i]);
      }

      while (1);*/

    for (i = 0x100; i != 0x110; i++){
	page_table[i] = (i << 12) | 0x013;
	//printk ("%x\n", page_table[i]);
    }

    // make a mapping for video memory
    page_table[184] = 0x000B801B;

    printk ("%x\n", &page_directory);
    asm volatile("mov %0, %%cr3":: "r"(&page_directory));
    asm volatile("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000; // Enable paging!
    asm volatile("mov %0, %%cr0":: "r"(cr0));

    //i = *(volatile unsigned*)0x00104000;
    return 0;
}

/*
  void create_page_directory_entry ()
  {
  }*/
