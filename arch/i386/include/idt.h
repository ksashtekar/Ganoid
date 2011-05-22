#ifndef IDT_H_
#define IDT_H_


#include <kernel.h>

struct interrupt_descriptor
{
	unsigned short offset_low;
	unsigned short segment_selector;
	unsigned char zero_area;
	unsigned char flags;
	unsigned short offset_high;
}__attribute__((packed));

struct IDTR_val
{
	unsigned short table_limit;
	unsigned table_base_addr;
} __attribute__((packed)); 

#define INT_DESC_GATESIZE32BIT (1<<3)
#define INT_DESC_GATESIZE16BIT (0<<3)

#define INT_DESC_SEG_PRESENT (1<<7)

void init_Interrupt_Descriptor_Table (void);
void add_idt_entry (int idt_index, unsigned short segment_selector, 
		    unsigned segment_offset, unsigned int descriptor_priv_level, 
		    unsigned int gate_size);
void print_interrupt_desc_table (const struct IDTR_val *IDTR_val);


#endif
