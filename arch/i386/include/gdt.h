#ifndef GDT_H_
#define GDT_H_

#include <ktypes.h>

#define DEFAULT_GDT_SIZE 6

void InitGDT (void);


struct GDTR_val
{
	unsigned short table_limit;
	unsigned table_base_addr;
} __attribute__((packed)); 

struct segment_descriptor
{
	unsigned short seg_limit_low;
	unsigned short base_addr_low;
	unsigned char base_addr_middle;
	unsigned char flags_1;
	unsigned char flags_2;
	unsigned char base_addr_high;
} __attribute__((packed));


enum codedata_segment_selector_TYPE
	{
		DATA_EXPAND_UP_RO = 0,
		DATA_EXPAND_UP_RW = 2,
		DATA_EXPAND_DOWN_RO = 4,
		DATA_EXPAND_DOWN_RW = 6,
		CODE_NO_CONFIRMING_NO_R = 8,
		CODE_NO_CONFIRMING_R = 10,
		CODE_CONFIRM_NO_R = 12,
		CODE_CONFIRM_R = 14
	};
 
enum sys_segment_selector_TYPE
	{
		SYS_RESERVED = 0,
		SYS_16BIT_TSS_AVALIABLE,
		SYS_LDT,
		SYS_16BIT_TSS_BUSY,
		SYS_16BIT_CALL_GATE,
		SYS_TASK_GATE,
		SYS_16BIT_INT_GATE,
		SYS_16BIT_TRAP_GATE,
		SYS_RESERVED_1,
		SYS_32BIT_TSS_AVAILABLE,
		SYS_RESERVED_2,
		SYS_32BIT_TSS_BUSY,
		SYS_32BIT_CALL_GATE,
		SYS_RESERVED_3,
		SYS_32BIT_INT_GATE,
		SYS_32BIT_TRAP_GATE
	};

enum descriptor_type 
	{
		DESC_SYSTEM = 0,
		DESC_CODEDATA
	};

#define GBL_DESC_DEFAULT_OPERATION_SIZE_32BIT (1<<6)
#define GBL_DESC_GRANULARITY_4KB (1<<7) 
#define GBL_DESC_SEGMENT_PRESENT (1<<7)

void init_GDT (void);
void add_gdt_entry (int gdt_index, unsigned seg_base, unsigned seg_limit,
		   int seg_type, bool desc_type, int desc_priv_level,
		    bool default_operation_size, bool granularity);
void print_global_desc_table (const struct GDTR_val *GDTR_val);

#endif // GDT_H_
