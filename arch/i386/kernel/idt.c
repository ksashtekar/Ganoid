#include <stdarg.h>
#include <kernel.h>
#include <string.h>
#include <idt.h>
#include <vectors.h>

#define TOTAL_IDT_ENTRIES 256

void isr_handler (void);

struct interrupt_descriptor IDT[TOTAL_IDT_ENTRIES];
struct IDTR_val IDTR_val;

void init_Interrupt_Descriptor_Table (void)
{
	int i;
	
	struct IDTR_val cur_IDTR_val;
	memset (&IDT, 0, sizeof(IDT));

	add_idt_entry (0, 0x08, (unsigned)ISR0, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (1, 0x08, (unsigned)ISR1, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (2, 0x08, (unsigned)ISR2, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (3, 0x08, (unsigned)ISR3, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (4, 0x08, (unsigned)ISR4, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (5, 0x08, (unsigned)ISR5, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (6, 0x08, (unsigned)ISR6, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (7, 0x08, (unsigned)ISR7, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (8, 0x08, (unsigned)ISR8, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (9, 0x08, (unsigned)ISR9, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (10, 0x08, (unsigned)ISR10, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (11, 0x08, (unsigned)ISR11, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (12, 0x08, (unsigned)ISR12, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (13, 0x08, (unsigned)ISR13, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (14, 0x08, (unsigned)ISR14, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (15, 0x08, (unsigned)ISR15, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (16, 0x08, (unsigned)ISR16, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (17, 0x08, (unsigned)ISR17, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (18, 0x08, (unsigned)ISR18, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (19, 0x08, (unsigned)ISR19, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (20, 0x08, (unsigned)ISR20, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (21, 0x08, (unsigned)ISR21, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (22, 0x08, (unsigned)ISR22, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (23, 0x08, (unsigned)ISR23, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (24, 0x08, (unsigned)ISR24, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (25, 0x08, (unsigned)ISR25, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (26, 0x08, (unsigned)ISR26, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (27, 0x08, (unsigned)ISR27, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (28, 0x08, (unsigned)ISR28, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (29, 0x08, (unsigned)ISR29, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (30, 0x08, (unsigned)ISR30, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (31, 0x08, (unsigned)ISR31, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (32, 0x08, (unsigned)ISR32, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (33, 0x08, (unsigned)ISR33, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (34, 0x08, (unsigned)ISR34, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (35, 0x08, (unsigned)ISR35, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (36, 0x08, (unsigned)ISR36, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (37, 0x08, (unsigned)ISR37, DESC_SVC, INT_DESC_GATESIZE32BIT);
	add_idt_entry (38, 0x08, (unsigned)ISR38, DESC_SVC, INT_DESC_GATESIZE32BIT);
	//add_idt_entry (39, 0x08, (unsigned)ISR31, DESC_SVC, INT_DESC_GATESIZE32BIT);
	//add_idt_entry (40, 0x08, (unsigned)ISR31, DESC_SVC, INT_DESC_GATESIZE32BIT);

	// add for remaining
	for (i = 39; i < 256; i++)
		add_idt_entry (i, 0x08, (unsigned)ISR20, DESC_SVC, INT_DESC_GATESIZE32BIT);



	IDTR_val.table_base_addr = (unsigned int)&IDT;
	IDTR_val.table_limit = (sizeof(IDT)) - 1;       

	
	asm volatile ("lidt %0":"=m"(IDTR_val));
	//print_interrupt_desc_table (&cur_IDTR_val);
	//while(1);
}


void add_idt_entry (int idt_index, unsigned short segment_selector, 
		    unsigned segment_offset, int descriptor_priv_level, 
		    int gate_size)
{
	IDT[idt_index].offset_low = segment_offset & 0x0000FFFF;
	IDT[idt_index].segment_selector = segment_selector;
	IDT[idt_index].zero_area = 0;
	IDT[idt_index].flags = 0x80 | (DESC_SVC << 5) | 
		INT_DESC_GATESIZE32BIT | 0x07;
	IDT[idt_index].offset_high = (segment_offset & 0xFFFF0000) >> 16;
}


void print_interrupt_desc_table (const struct IDTR_val *IDTR_val)
{
	int i;
	struct interrupt_descriptor *int_desc = (struct interrupt_descriptor*)IDTR_val->table_base_addr;
	unsigned short segment_selector;
	unsigned int segment_offset;

	// seg_desc points to the first segment descriptor
	for (i=0; i < IDTR_val->table_limit; i+=8, int_desc+=1)	{
		printf ("\nInterrupt Descriptor No.: %d\n", i/8);
		printf ("Descriptor Address: %x\n", int_desc);
		printf ("DWord_H: %8x\n", *(unsigned int*)((unsigned int*)int_desc+1));
		printf ("DWord_L: %8x\n", *(unsigned int*)int_desc);
		segment_selector = int_desc->segment_selector;
		segment_offset = ((unsigned int)int_desc->offset_high)<<16 | 
			(unsigned int)int_desc->offset_low;
		printf ("Segment selector: %8x\n", segment_selector);
		printf ("Segment offset: %8x\n", segment_offset);
		delay (18000000);
	}
}
















