/* 
 * This file defines and implements the Global descriptor tables.
 */


#include <ganoid/types.h>
#include <string.h>
#include <gdt.h>
#include <kernel.h>

void gdt_flush (void);

// allocate space for the GDT

struct segment_descriptor GDT[DEFAULT_GDT_SIZE] __attribute__((aligned(8)));
struct GDTR_val GDTR_val;

void init_GDT (void)
{
	//struct GDTR_val cur_gdtr_val;

	// clear the global descriptor table.
	memset (GDT, 0, (sizeof(struct segment_descriptor)*DEFAULT_GDT_SIZE));

	// keep first entry (entry 0) empty)
	
	// code segment
	add_gdt_entry (1, 0, 0xFFFFF, 
		       CODE_NO_CONFIRMING_NO_R, DESC_CODEDATA, DESC_SVC,
		       GBL_DESC_DEFAULT_OPERATION_SIZE_32BIT, GBL_DESC_GRANULARITY_4KB);
	// data segment
	add_gdt_entry (2, 0, 0xFFFFF, 
		       DATA_EXPAND_UP_RW, DESC_CODEDATA, DESC_SVC,
		       GBL_DESC_DEFAULT_OPERATION_SIZE_32BIT, GBL_DESC_GRANULARITY_4KB);
	/*
	// stack segment
	add_gdt_entry (3, 0xFFFFFFFF, 0xFFFFF, 
		       DATA_EXPAND_DOWN_RW, DESC_CODEDATA, DESC_SVC,
		       GBL_DESC_DEFAULT_OPERATION_SIZE_32BIT, GBL_DESC_GRANULARITY_4KB);
	*/
	
	GDTR_val.table_limit = (sizeof(struct segment_descriptor)*DEFAULT_GDT_SIZE);
	GDTR_val.table_base_addr = (unsigned)&GDT;
	gdt_flush ();

	/*
	// test
	asm volatile ("sgdt %0": :"m"(cur_gdtr_val));
	print_global_desc_table (&cur_gdtr_val);
	while (1);
	*/
}

/* 
 * Encode the given information and create descriptor in memory.
 * Activate it immediately.
 */

void add_gdt_entry (int gdt_index, unsigned seg_base, unsigned seg_limit,
		   int seg_type, bool desc_type, int desc_priv_level,
		   bool default_operation_size, bool granularity)

{
	GDT[gdt_index].seg_limit_low = (seg_limit & 0xFFFF);
	GDT[gdt_index].base_addr_low = (seg_base & 0xFFFF);
	GDT[gdt_index].base_addr_middle =(unsigned char)((seg_base & 0x00FF0000)>>16);
	GDT[gdt_index].flags_1 = (unsigned char)((GBL_DESC_SEGMENT_PRESENT) 
						 | (desc_priv_level << 5) | (desc_type << 4) 
						 | (seg_type));
	GDT[gdt_index].flags_2 =(unsigned char)((u32)granularity  | (u32)default_operation_size
						| (u32)((seg_limit & 0x000F0000) >> 16));
	GDT[gdt_index].base_addr_high = (unsigned char)((seg_base & 0xFF000000) >> 24);
}



void print_global_desc_table (const struct GDTR_val *GDTR_value)
{
	int i;
	struct segment_descriptor *seg_desc = (struct segment_descriptor*)GDTR_value->table_base_addr;
	unsigned int segment_base_address;
	unsigned int segment_limit;

	// test code
	unsigned char *ptr;
	ptr = (unsigned char*)seg_desc;
	/*
	for (i = 0; i <= GDTR_value->table_limit; i++){
		printk ("%2x ", *ptr);
		ptr++;
		if (!((i+1)%4))
			printk ("\n");
		//delay (1000000);
	}
	*/

	// seg_desc points to the first segment descriptor
	for (i=0; i < GDTR_value->table_limit; i+=8, seg_desc+=1)	{
		printk ("\nSegment Descriptor No.: %d\n", i/8);
		printk ("Descriptor Address: %x\n", seg_desc);
		printk ("DWord_H: %8x\n", *(unsigned int*)((unsigned int*)seg_desc+1));
		printk ("DWord_L: %8x\n", *(unsigned int*)seg_desc);
		segment_base_address = ((unsigned int)seg_desc->base_addr_high)<<24 |
			((unsigned int)seg_desc->base_addr_middle)<<16 |
			((unsigned int)seg_desc->base_addr_low);
		segment_limit = ((((unsigned int)(seg_desc->flags_2)) & 0x0F)<<16) |
			(unsigned int)seg_desc->seg_limit_low;
		printk ("Segment Base Address: %8x\n", segment_base_address);
		printk ("Segment Limit: %8x\n", segment_limit);
		delay (18000000);
	}
}


