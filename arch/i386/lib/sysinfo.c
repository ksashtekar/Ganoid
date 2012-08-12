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
#include <utils.h>
#include <multiboot.h>

#define ADDR_MAP_NODE_CNT 20
#define DRIVE_INFO_NODE_CNT 20

static struct multiboot_info mi;
static struct multiboot_mmap_entry mboot_mmap[ADDR_MAP_NODE_CNT];
static struct bios_drive_info drive_info[DRIVE_INFO_NODE_CNT];
static ram_map_t ram_map[ADDR_MAP_NODE_CNT];

// ~temp
extern int debug_printk;
static unsigned ram_nodes_cnt;

static void parse_mboot_information(void)
{
	const char na[] = "Not Specified";
	u32 flags = mi.flags;
	printk("Flags: 0x%x\n", flags);
	const char *drivestr[] = {
		"1st floppy disk\0",
		"2nd floppy disk\0",
		"1st hard disk\0",
		"2nd hard disk\0" "unknown medium\0"
	};

	printk("Amount of lower memory: ");
	if (flags & MULTIBOOT_INFO_MEMORY) {
		printk("%d KB\n", mi.mem_lower);
		printk("Amount of upper memory: %d KB\n", mi.mem_upper);
	} else {
		printk("%s\n", na);
	}

	u32 drive;
	if (flags & MULTIBOOT_INFO_BOOTDEV) {
		drive = (mi.boot_device & 0xFF000000) >> 24;
		printk("Bios drive number: %x: ", drive);
		const char *str;
		switch (drive) {
		case 0:
			str = drivestr[0];
			break;
		case 1:
			str = drivestr[1];
			break;
		case 0x80:
			str = drivestr[2];
			break;
		case 0x81:
			str = drivestr[3];
			break;
		default:
			str = drivestr[4];
			break;
		}

		printk("%s\n", str);
		printk("part1: %x\n", (mi.boot_device & 0x00FF0000) >> 16);
		printk("part2: %x\n", ((mi.boot_device & 0x0000FF00) >> 8));
		printk("part3: %x\n", (mi.boot_device & 0x000000FF));
	} else
		printk("Boot device unknown\n");

	if (flags & MULTIBOOT_INFO_CMDLINE)
		printk("Command line: %s\n", (char *)mi.cmdline);
	else
		printk("Command line data not present\n");

	printk("Module information NOT parsed\n");
	printk("Symbol table information not parsed\n");

	printk("RAM Map: Flags: %x mi.mmap_length: %d\n",
	       flags, mi.mmap_length);
	if ((flags & MULTIBOOT_INFO_MEM_MAP) && mi.mmap_length) {
		const u32 mask32 = 0xFFFFFFFF;
		unsigned i, node_cnt =
		    mi.mmap_length / sizeof(struct multiboot_mmap_entry);
		ram_nodes_cnt = 0;
		for (i = 0; i < node_cnt; i++) {
			//printk("******* Node %d ********\n", i);
			//printk("Node size    : %d\n", mboot_mmap[i].node_size);
			if (mboot_mmap[i].type == MULTIBOOT_MEMORY_AVAILABLE) {
				printk("RAM     : ");
				ram_map[ram_nodes_cnt].saddr =
				    (u32) (mboot_mmap[i].addr & mask32);
				ram_map[ram_nodes_cnt].len =
				    (unsigned)(mboot_mmap[i].len & mask32);
				ram_nodes_cnt++;
			} else
				printk("Reserved: ");
			printk("0x%8x ",
			       (unsigned)((unsigned)mboot_mmap[i].addr &
					  mask32));
			print_human_readable_size((unsigned)
						  ((unsigned)mboot_mmap[i].len &
						   mask32));
		}
	}

	if (flags & MULTIBOOT_INFO_DRIVE_INFO) {
		unsigned i = 0, node_cnt =
		    mi.drives_length / sizeof(struct bios_drive_info);
		for (i = 0; i < node_cnt; i++) {
			//printk("********** Node %d *********\n", i++);
			//printk("%d", drive_info[i].size);
			printk("%d", drive_info[i].drive_no);
			if (drive_info[i].drive_mode == 0)
				printk(", CHS");
			else if (drive_info[i].drive_mode == 1)
				printk(", LBA mode");
			else
				printk(", Unknown mode");

			printk(", %d-C", drive_info[i].drive_cylinders);
			printk(", %d-H", drive_info[i].drive_heads);
			printk(", %d-S", drive_info[i].drive_sectors);
			u16 *start_port_array = drive_info[i].start_port_array;
			while (*start_port_array) {
				printk(", %d", *start_port_array);
				start_port_array++;
			}
			printk("\n");;
		}
	}

	printk("ROM configuration table not parsed!\n");

	if (flags & MULTIBOOT_INFO_BOOT_LOADER_NAME)
		printk("Bootloader name: %s\n", (char *)mi.boot_loader_name);

	printk("APM information not parsed\n");

	printk("VBE information not parsed\n");
}

/* Parse the multiboot information.  
 * 
 * The information about the system hardware environment
 * provided by multiboot needs to be saved somewhere as it
 * may get destroyed in due course of booting. Also, we need
 * to parse it and make it available to anybody needing
 * it. We will parse it here.
 */
void read_multiboot_information(u32 * multiboot_info_ptr)
{
	// read in the entire struct
	memcpy(&mi, multiboot_info_ptr, sizeof(struct multiboot_info));

	// read the mem map information
	if (mi.mmap_length > sizeof(mboot_mmap))
		kernel_fault("mboot_mmap space insufficient");
	if (mi.mmap_addr)
		memcpy(&mboot_mmap, (void *)mi.mmap_addr, mi.mmap_length);

	if (mi.drives_length > sizeof(drive_info))
		kernel_fault("drive_info not sufficient");
	// read the drive information
	if (mi.drives_addr)
		memcpy(&drive_info, (void *)mi.drives_addr, mi.drives_length);

	parse_mboot_information();
}

const struct multiboot_mmap_entry *get_bios_addr_buffer(unsigned *node_cnt)
{
	*node_cnt = mi.mmap_length / sizeof(struct multiboot_mmap_entry);
	return mboot_mmap;
}

void display_boot_progress(const char *message, bool result)
{
#define MAX_STR_LEN 200
	char column_str[MAX_STR_LEN];
#undef MAX_STR_LEN
	const char *ok_str = "OK";
	const char *fail_str = "FAIL";
	const char *pstr = ok_str;
	const int right_margin = 8;
	const int left_margin = 1;
	columnlize_string(message, column_str, SCREEN_WIDTH,
			  left_margin, right_margin, '.');

	printk("%s", column_str);

	if (result)
		pstr = fail_str;

	for (int i = 0; i < (right_margin - strlen(pstr) - 2); i++)
		printk(".");

	printk("[%s]", pstr);

	//  while(1);
}

// str should be of enough length to fit entire columnized
// string. 
// will try to put tabs if possible instead of space to save space
char *columnlize_string(const char *istr, char *ostr, int screen_width,
			int left_margin, int right_margin, char fillbyte)
{
	int tabs = left_margin / KTAB_WIDTH;
	int leftfillspace = left_margin % KTAB_WIDTH;
	int single_line_str_len = screen_width - (left_margin + right_margin);
	if (single_line_str_len < 1)
		return NULL;

	// temptest
	//single_line_str_len = 3;

	char *obuf;
	obuf = ostr;

	int r = 0;

	for (; *istr;) {
		if (left_margin) {
			for (int j = 0; j < tabs; j++)
				sprintf(ostr++, "\t");
			for (int k = 0; k < leftfillspace; k++)
				sprintf(ostr++, " ");
		}

		for (r = 0;
		     (r < single_line_str_len) && (*istr) && (*istr != '\n');
		     r++)
			*ostr++ = *istr++;

		if (*istr == '\n')
			istr++;

		if (!*istr)
			break;	// we are done with

		*ostr = '\n';
		ostr++;
	}

	if (r < single_line_str_len)
		for (int m = 0; m < (single_line_str_len - r); m++, ostr++)
			*ostr = fillbyte;

	*ostr = 0;

	return obuf;
}

const ram_map_t *get_rammap_ptr(unsigned *rammap_nodes)
{
	*rammap_nodes = ram_nodes_cnt;
	return ram_map;
}
