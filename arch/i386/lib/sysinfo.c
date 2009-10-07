#include <ktypes.h>
#include <string.h>
#include <kdebug.h>
#include <sysinfo.h>
#include <kernel.h>


#define ADDR_MAP_NO_OF_NODES 20
#define BIOS_DRIVE_INFO_NO_OF_NODES 20


static struct multiboot_info multiboot_info;
static u8 bios_addr_map_buffer[sizeof(struct bios_addr_map)*ADDR_MAP_NO_OF_NODES];
static u8 bios_drive_info_buffer[sizeof(struct bios_drive_info)*BIOS_DRIVE_INFO_NO_OF_NODES];

/* Parse the multiboot information.  
 * 
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

	//for (int i = 0; i < (sizeof(multiboot_info)/4); i++)
	//printf ("%d: %x\n", i*4, *((u32*)&multiboot_info+i));

	print_multiboot_information ();

	while (1);
}


static void print_multiboot_information (void)
{
	char na[]="Not Specified";
	u32 flags;
	flags = multiboot_info.flags;
	printf ("Flags: 0x%x\n", flags);
	char *drivestr[] = {
		"1st floppy disk\0",
		"2nd floppy disk\0",
		"1st hard disk\0",
		"2nd hard disk\0"
		"unknown medium\0"
	};
	u32 drive;

	char array[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	data_dump ((void*)0x16, (void*)0x1b, EDUMP_HALFWORD, EDUMP_HEX);
	while (1); 

	printf ("Amount of lower memory: ");
	if (flags & 0x01) {
		printf ("%d KB\n", multiboot_info.mem_lower);
		printf ("Amount of upper memory: %d KB\n", multiboot_info.mem_upper);
	}
	else{
		printf ("%s\n", na);
	}

	if (flags & 0x02) {
		drive = (multiboot_info.boot_device & 0xFF000000)>>24;
		printf ("Bios drive number: %x: ", drive);
		char *str;
		switch (drive){ 
		case 0: str = drivestr[0]; break;
		case 1: str = drivestr[1]; break;
		case 0x80: str = drivestr[2]; break;
		case 0x81: str = drivestr[3]; break;
		default: str = drivestr[4]; break;
		}


		printf ("%s\n", str);
		printf ("part1: %x\n", ((multiboot_info.boot_device & 0x00FF0000)>>16));
		printf ("part2: %x\n", ((multiboot_info.boot_device & 0x0000FF00)>>8));
		printf ("part3: %x\n", (multiboot_info.boot_device & 0x000000FF));
	}
	else
		printf ("Boot device unknown\n");

	if (flags & 0x04) 
		printf ("Command line: %s\n", (char*)multiboot_info.cmdline);
	else
		printf ("Command line data not present\n");


	printf ("kaustubh..%s\n", drivestr[0]);
	while (1);


	printf ("Module information NOT parsed for now\n");
	printf ("Symbol table information not parsed\n");

	if ((flags & 0x20) && multiboot_info.mmap_length){
		struct bios_addr_map *bios_addr_map;
		u32 i = 0;
		u32 size = multiboot_info.mmap_length;
		bios_addr_map = (struct bios_addr_map*)bios_addr_map_buffer;
		while (size){
			i++;
			printf ("******* Node %d ********\n", i);
			printf ("Base address : 0x%x%x\n", bios_addr_map->base_addr_high, 
				bios_addr_map->base_addr_low);
			printf ("Length       : 0x%x%x bytes\n", bios_addr_map->length_high,
				bios_addr_map->length_low);
			printf ("Type         : %x\n", bios_addr_map->type);
			size -= bios_addr_map->node_size;
			bios_addr_map = (struct bios_addr_map*)((u32)bios_addr_map + 
								  bios_addr_map->node_size);
		}
	}

	if (flags & 0x80){
		struct bios_drive_info *bios_drive_info = 
			(struct bios_drive_info*)multiboot_info.drives_addr;

		u32 i = 0;
		u32 size = multiboot_info.drives_length;
		while(size){
			printf ("********** Node %d *********\n", i++);
			printf ("Drive number: %d\n", bios_drive_info->drive_no);
			printf ("Drive Mode: ");
			if (bios_drive_info->drive_mode == 0) printf ("CHS\n");
			else if (bios_drive_info ->drive_mode == 1) printf ("LBA mode\n");
			
			printf ("Drive cylinders: %d\n", bios_drive_info->drive_cylinders);
			printf ("Drive Heads:     %d\n", bios_drive_info->drive_heads);
			printf ("Drive Sectors    %d\n", bios_drive_info->drive_sectors);
			u16 *start_port_array = &bios_drive_info->start_port_array;
			while (*start_port_array){
				printf ("Port: %d\n", *start_port_array);
				start_port_array++;
			}
			size = size - bios_drive_info->size;
		}
	}

	printf ("ROM configuration table not parsed!\n");

	if (flags & 0x200)
		printf ("Bootloader name: %s\n", (char*)multiboot_info.boot_loader_name);

	printf ("APM information not parsed\n");

	printf ("VBE information not parsed\n");
}





