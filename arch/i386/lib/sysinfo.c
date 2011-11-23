/*********************************************************************
 * This file implements the parsing of the multiboot information 
 * structures. Since this to be done right at system startup we neither
 * kernel memory allocator nor the bootmem allocator. Thus we 
 * have to use memory in quite primitive way.
 *
 ********************************************************************/

#include <ganoid/types.h>
#include <string.h>
#include <kdebug.h>
#include <sysinfo.h>
#include <kernel.h>
#include <clib.h>
#include <vga.h>


#define ADDR_MAP_NO_OF_NODES 20
#define BIOS_DRIVE_INFO_NO_OF_NODES 20
#define RAM_MAP_NODES 5


static struct multiboot_info multiboot_info;
static char bios_addr_map_buffer[sizeof(struct bios_addr_map)*ADDR_MAP_NO_OF_NODES];
static u8 bios_drive_info_buffer[sizeof(struct bios_drive_info)*BIOS_DRIVE_INFO_NO_OF_NODES];
static ram_map ram_map_store[RAM_MAP_NODES];

static void parse_multiboot_information (void);

// ~temp
extern int debug_printk;


static u32 ram_nodes_cnt = 0;


/* Parse the multiboot information.  
 * 
 * The information about the system hardware environment
 * provided by multiboot needs to be saved somewhere as it
 * may get destroyed in due course of booting. Also, we need
 * to parse it and make it available to anybody needing
 * it. We will parse it here.
 */
void read_multiboot_information (u32 *multiboot_info_ptr)
{
	// read in the entire struct
	memcpy (&multiboot_info, multiboot_info_ptr, sizeof(struct multiboot_info));

	// read the mem map information
	if (multiboot_info.mmap_length > sizeof(bios_addr_map_buffer))
		kernel_fault ("bios_addr_map_buffer space insufficient");
	if (multiboot_info.mmap_addr)
		memcpy (bios_addr_map_buffer, (u8*)multiboot_info.mmap_addr, 
			multiboot_info.mmap_length);

	
	if (multiboot_info.drives_length > sizeof(bios_drive_info_buffer))
		kernel_fault ("bios_drive_info_buffer not sufficient");
	// read the drive information
	if (multiboot_info.drives_addr)
		memcpy (bios_drive_info_buffer, (u8*)multiboot_info.drives_addr,
			multiboot_info.drives_length);


	parse_multiboot_information ();

	//while (1);
}


const char* get_bios_addr_buffer (u32 *size)
{
	*size = multiboot_info.mmap_length;
	return bios_addr_map_buffer;
}


static void parse_multiboot_information (void)
{
	char na[]="Not Specified";
	u32 flags;
	flags = multiboot_info.flags;
	printk ("Flags: 0x%x\n", flags);
	const char *drivestr[] = {
		"1st floppy disk\0",
		"2nd floppy disk\0",
		"1st hard disk\0",
		"2nd hard disk\0"
		"unknown medium\0"
	};
	u32 drive;

	printk ("Amount of lower memory: ");
	if (flags & 0x01) {
		printk ("%d KB\n", multiboot_info.mem_lower);
		printk ("Amount of upper memory: %d KB\n", multiboot_info.mem_upper);
	}
	else{
		printk ("%s\n", na);
	}

	if (flags & 0x02) {
		drive = (multiboot_info.boot_device & 0xFF000000)>>24;
		printk ("Bios drive number: %x: ", drive);
		const char *str;
		switch (drive){ 
		case 0: str = drivestr[0]; break;
		case 1: str = drivestr[1]; break;
		case 0x80: str = drivestr[2]; break;
		case 0x81: str = drivestr[3]; break;
		default: str = drivestr[4]; break;
		}


		printk ("%s\n", str);
		printk ("part1: %x\n", ((multiboot_info.boot_device & 0x00FF0000)>>16));
		printk ("part2: %x\n", ((multiboot_info.boot_device & 0x0000FF00)>>8));
		printk ("part3: %x\n", (multiboot_info.boot_device & 0x000000FF));
	}
	else
		printk ("Boot device unknown\n");

	if (flags & 0x04) 
		printk ("Command line: %s\n", (char*)multiboot_info.cmdline);
	else
		printk ("Command line data not present\n");

	printk ("Module information NOT parsed for now\n");
	printk ("Symbol table information not parsed\n");

	if ((flags & 0x20) && multiboot_info.mmap_length){
		struct bios_addr_map *bios_addr_map;
		u32 i = 0;
		u32 size = multiboot_info.mmap_length;
		bios_addr_map = (struct bios_addr_map*)bios_addr_map_buffer;
		while (size){
			i++;
			//printk ("******* Node %d ********\n", i);
			//printk ("Node address : 0x%x\n", bios_addr_map);
			//printk ("Node size    : %d\n", bios_addr_map->node_size);
			printk ("0x%x%8x", bios_addr_map->base_addr_high, 
				bios_addr_map->base_addr_low);
			printk (", 0x%x%8x ", bios_addr_map->length_high,
				bios_addr_map->length_low, bios_addr_map->length_low/1000);
			if (bios_addr_map->length_low/1000000000)
				printk ("(%d) GB", bios_addr_map->length_low/1000000000);
			else if (bios_addr_map->length_low/1000000)
				printk ("(%d) MB", bios_addr_map->length_low/1000000);
			else if (bios_addr_map->length_low/1000)
				printk ("(%d) KB", bios_addr_map->length_low/1000);
			else
				printk ("(%d) Bytes", bios_addr_map->length_low);

			if (bios_addr_map->type == MULTIBOOT_RAM){
				printk (", RAM\n");
				_ASSERT((ram_nodes_cnt<RAM_MAP_NODES),EMultiBootSpaceInsufficient,
					ram_nodes_cnt, RAM_MAP_NODES, i);
				ram_map_store[ram_nodes_cnt].start_addr = bios_addr_map->base_addr_low;
				ram_map_store[ram_nodes_cnt].end_addr = (u32)bios_addr_map->base_addr_low 
					+ (u32)bios_addr_map->length_low;
				ram_nodes_cnt++;
			}
			else
				printk (", Reserved\n");

			size -= (bios_addr_map->node_size + sizeof (bios_addr_map->node_size));
			bios_addr_map = (struct bios_addr_map*)((u32)bios_addr_map + 
								  bios_addr_map->node_size + 
								sizeof(bios_addr_map->node_size) );
		}
	}

	if (flags & 0x80){
		struct bios_drive_info *bios_drive_info = 
			(struct bios_drive_info*)multiboot_info.drives_addr;

		u32 i = 0;
		u32 size = multiboot_info.drives_length;
		//printk ("total size: %d\n", size);
		while(size){
			i++;
			//printk ("********** Node %d *********\n", i++);
			//printk ("%d", bios_drive_info->size);
			printk ("%d", bios_drive_info->drive_no);
			if (bios_drive_info->drive_mode == 0) printk (", CHS");
			else if (bios_drive_info ->drive_mode == 1) printk (", LBA mode");
			else printk (", Unknown mode");
			
			printk (", %d-C", bios_drive_info->drive_cylinders);
			printk (", %d-H", bios_drive_info->drive_heads);
			printk (", %d-S", bios_drive_info->drive_sectors);
			u16 *start_port_array = &bios_drive_info->start_port_array;
			while (*start_port_array){
				printk (", %d", *start_port_array);
				start_port_array++;
			}
			printk ("\n");;

			size -= bios_drive_info->size;
			//printk  ("size remaining %d", size);
		}
	}

	printk ("ROM configuration table not parsed!\n");

	if (flags & 0x200)
		printk ("Bootloader name: %s\n", (char*)multiboot_info.boot_loader_name);

	printk ("APM information not parsed\n");

	printk ("VBE information not parsed\n");


}


void display_boot_progress (const char *message, bool result)
{
#define MAX_STR_LEN 200
	char column_str[MAX_STR_LEN];
#undef MAX_STR_LEN
	char *ok_str = "OK";
	char *fail_str = "FAIL";
	char *pstr = ok_str;
	const int right_margin = 8;
	const left_margin = 1;
	columnlize_string (message, column_str, SCREEN_WIDTH, 
			   left_margin, right_margin, '.');

	printk ("%s", column_str);
	
	if (result)
		pstr = fail_str;
	
	for (int i = 0; i < (right_margin - strlen(pstr) - 2); i++)
		printk (".");

	printk ("[%s]", pstr);

	//	while (1);
}


// str should be of enough length to fit entire columnized
// string. 
// will try to put tabs if possible instead of space to save space
char* columnlize_string (const char *istr, char *ostr, int screen_width, 
			 int left_margin, int right_margin, int fillbyte)
{
	int tabs = left_margin/KTAB_WIDTH;
	int leftfillspace = left_margin%KTAB_WIDTH;
	int i = -1;
	int single_line_str_len = screen_width - (left_margin + right_margin);
	if (single_line_str_len < 1)
		return NULL;

	// temptest
	//single_line_str_len = 3;

	char *obuf;
	obuf = ostr;

	int r = 0;

	int ip = 0, op = 0;

	for (ip = 0, op = 0; *istr; ){
		if (left_margin) {
			for (int i = 0; i < tabs; i++) sprintf(ostr++, "\t");
			for (int i = 0; i < leftfillspace; i++) sprintf(ostr++, " ");
		}

		for (r = 0; (r < single_line_str_len) && (*istr) && (*istr != '\n'); r++)
			*ostr++ = *istr++;

		if (*istr == '\n')
			istr++;

		if (!*istr)
			break; // we are done with

		*ostr = '\n';
		ostr++;
	}

	if (r < single_line_str_len)
		for (int i = 0; i < (single_line_str_len - r); i++, ostr++)
			*ostr = fillbyte;

	*ostr = 0;

	return obuf;
}



const ram_map* get_rammap_ptr (u32 *rammap_nodes)
{
	*rammap_nodes = ram_nodes_cnt;
	return ram_map_store;
}
