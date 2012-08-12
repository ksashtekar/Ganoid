#include <ganoid/types.h>
#include <kernel.h>
#include <kdebug.h>

/* apart from ASCII */
static const u32 line_width = 16;

static const char debug_strings[][1000] = {
	"Multiboot returned misaligned RAM address",	/* 0 */
	"Bootmem allocator could not find enough free space for heap",	/* 1 */
	"Bootmem allocator could not allocate memory for accounting",	/* 2 */
	"Multiboot data seems to be corrupted",	/* 3 */
	"Multiboot could not find enough space for parsing of data",	/* 4 */
	"Free address received by bootmem allocator is not valid",	/* 5 */
	"The bootmemory allocator is not initialized yet.",	/* 6 */
	"The IRQ dispatcher is not initialized yet!",	/* 7 */
	"NULL pointer is not allowed here",	/* 8 */
	"NULL"
};

void kernel_fault(const char *str)
{
	printk("KERN_FAULT: %s\n", str);
	while (1)
		;
}

void data_dump_byte(void *start_addr)
{
	data_dump(start_addr, (void *)~0, EDUMP_BYTE, EDUMP_HEX);
}

void data_dump_word(void *start_addr)
{
	data_dump(start_addr, (void *)~0, EDUMP_WORD, EDUMP_HEX);
}

void data_dump_halfword(void *start_addr)
{
	data_dump(start_addr, (void *)~0, EDUMP_HALFWORD, EDUMP_HEX);
}

void data_dump(void *start_addr, void *end_addr,
	       enum kdebug_dump_format_width width,
	       enum kdebug_dump_format_type type __attribute__ ((unused)))
{
	u32 elements_per_line, addr_print;
	u8 misalign_offset;
	int fill = -1;

	/* round-off start address to 16 bytes boundary */
	addr_print = ((u32) start_addr & (u32) 0xFFFFFFF0);
	misalign_offset = (u8) ((u32) start_addr & 0xF);

	switch (width) {
	case EDUMP_BYTE:
		elements_per_line = 16;
		fill = 1;
		break;
	case EDUMP_HALFWORD:
		elements_per_line = 8;
		fill = 3;
		break;
	case EDUMP_WORD:
		elements_per_line = 4;
		fill = 7;
		break;
	}

	printk("**************** Kernel debug data dump ***************\n");
	unsigned n = line_width / elements_per_line;
	for (unsigned s = 0; s < 12; s++)
		printk(" ");
	for (unsigned i = 0; i != 16; i += n) {
		printk("x%x", i);
		for (int s = 0; s < fill; s++)
			printk(" ");
	}

	/* printk("   ASCII\n"); */

	int point_printed = 0;
	int skip;
	for (u32 i = 0; i < 10; i++) {
		printk("\n0x%8x: ", addr_print);
		for (unsigned j = 0; j < line_width; j += n) {
			u32 addr = (u32) (addr_print | j);
			if (addr > (u32) end_addr) {
				i = 20000;
				break;
			}
			if ((u32) start_addr >= (addr + n))
				skip = 1;
			else
				skip = 0;

			switch (width) {
			case EDUMP_BYTE:
				if (skip)
					printk("xx");
				else
					printk("%2x", *(u8 *) addr);
				break;
			case EDUMP_HALFWORD:
				if (skip)
					printk("xxxx");
				else
					printk("%4x", *(u16 *) addr);
				break;
			case EDUMP_WORD:
				if (skip)
					printk("xxxxxxxx");
				else
					printk("%8x", *(u32 *) addr);
				break;
			}
			if (!point_printed) {
				if ((misalign_offset >= j) &&
				    (misalign_offset < (j + n))) {
					point_printed = 1;
					printk("^");
				} else {
					printk(" ");
				}
			} else
				printk(" ");

			/*while (1); */
		}
		addr_print += 0x10;
	}

}

void display_error_info(const char *cond, int e,
			const char *n_v1, u32 v1,
			const char *n_v2, u32 v2,
			const char *n_v3, u32 v3,
			const char *file_name, u32 line_no)
{
	printk("\n\n\n\n\n");
	printk("*********************** Ganoid Fault ***********************\n");
	printk("File name     : %s\n", file_name);
	printk("Line No       : %d\n", line_no);
	printk("Check failed  : %s\n", cond);
	printk("Fault desc.   : %s\n", &debug_strings[e][0]);
	printk("Value 1       : 0x%8x (%s)\n", v1, n_v1);
	printk("Value 2       : 0x%8x (%s)\n", v2, n_v2);
	printk("Value 3       : 0x%8x (%s)\n", v3, n_v3);
	printk("************************************************************\n");
}
