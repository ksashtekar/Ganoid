#include <ktypes.h>
#include <kernel.h>
#include <kdebug.h>

// apart from ASCII
static const u32 line_width = 16;


void kernel_fault (const char *str)
{
	printf ("KERN_FAULT: %s\n", str);
	while (1);
}


void data_dump_byte (void *start_addr)
{
	data_dump (start_addr, ~0, EDUMP_BYTE, EDUMP_HEX);
}

void data_dump_word (void *start_addr)
{
	data_dump (start_addr, ~0, EDUMP_WORD, EDUMP_HEX);
}



void data_dump (void *start_addr, void *end_addr, 
		enum kdebug_dump_format_width width, 
		enum kdebug_dump_format_type type)
{
	u32 elements_per_line = -1;
	u32 addr_print = -1;
	u8  misalign_offset;
	int fill = -1;

	// round-off start address to 16 bytes boundary
	addr_print = ((u32)start_addr & (u32)0xFFFFFFF0);
	misalign_offset = (u8)((u32)start_addr & 0xF);

	switch (width){
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

	printf ("**************** Kernel debug data dump ***************\n");
	int n = line_width/elements_per_line;
	for (int s = 0; s < 12; s++)
		printf (" ");
	for (int i = 0; i != 16; i+=n){
		printf ("x%x", i);
		for (int s = 0; s < fill; s++)
			printf (" ");
	}

	//printf ("   ASCII  \n");

	int point_printed = 0;
	int skip;
	for (u32 i = 0;i < 10;i++){
		printf ("\n0x%8x: ", addr_print);
		for (int j = 0; j < line_width; j += n){
			u32 addr = (u32)(addr_print | j );
			if (addr > end_addr){
				i = 20000;
				break;
			}
			if (start_addr >= (addr+n))
				skip = 1;
			else
				skip = 0;


			switch (width){
			case EDUMP_BYTE:
				if (skip)
					printf ("xx");
				else
					printf ("%2x", *(u8*)addr);
				break;
			case EDUMP_HALFWORD:
				if (skip)
					printf ("xxxx");
				else
					printf ("%4x", *(u16*)addr);
				break;
			case EDUMP_WORD:
				if (skip)
					printf ("xxxxxxxx");
				else
					printf ("%8x", *(u32*)addr);
				break;
			}
			if (!point_printed){
				if ( (misalign_offset >= j) &&
				     (misalign_offset < (j + n) )){   
					point_printed = 1;
					printf ("^");
				}
				else{
					printf (" ");
				}
			}
			else
				printf (" ");

			//while (1);
		}
		addr_print += 0x10;
	}
	
}


