#ifndef SYSINFO_H_
#define SYSINFO_H_

#include <ktypes.h>


#define MULTIBOOT_RAM 1




/*
The format of the Multiboot information structure (as defined so far) follows:

             +-------------------+
     0       | flags             |    (required)
             +-------------------+
     4       | mem_lower         |    (present if flags[0] is set)
     8       | mem_upper         |    (present if flags[0] is set)
             +-------------------+
     12      | boot_device       |    (present if flags[1] is set)
             +-------------------+
     16      | cmdline           |    (present if flags[2] is set)
             +-------------------+
     20      | mods_count        |    (present if flags[3] is set)
     24      | mods_addr         |    (present if flags[3] is set)
             +-------------------+
     28 - 40 | syms              |    (present if flags[4] or
             |                   |                flags[5] is set)
             +-------------------+
     44      | mmap_length       |    (present if flags[6] is set)
     48      | mmap_addr         |    (present if flags[6] is set)
             +-------------------+
     52      | drives_length     |    (present if flags[7] is set)
     56      | drives_addr       |    (present if flags[7] is set)
             +-------------------+
     60      | config_table      |    (present if flags[8] is set)
             +-------------------+
     64      | boot_loader_name  |    (present if flags[9] is set)
             +-------------------+
     68      | apm_table         |    (present if flags[10] is set)
             +-------------------+
     72      | vbe_control_info  |    (present if flags[11] is set)
     76      | vbe_mode_info     |
     80      | vbe_mode          |
     82      | vbe_interface_seg |
     84      | vbe_interface_off |
     86      | vbe_interface_len |
             +-------------------+
*/


struct multiboot_a_out_info
{
	u32 tabsize;
	u32 strsize;
	u32 addr;
	u32 reserved;
};

struct multiboot_elf_info
{
	u32 num;
	u32 size;
	u32 addr;
	u32 shndx;
};

union syms 
{
	struct multiboot_a_out_info a_out_info;
	struct multiboot_elf_info elf_info;
};

struct multiboot_info 
{
	int flags;
        u32 mem_lower; //    (present if flags[0] is set)
        u32 mem_upper; //    (present if flags[0] is set)
        u32 boot_device; //  (present if flags[1] is set)
        u32 cmdline; //      (present if flags[2] is set)
        u32 mods_count; //   (present if flags[3] is set)
        u32 mods_addr;  //   (present if flags[3] is set)
	union syms syms; // (present if flags[4] or flags[5] is set)
        u32 mmap_length; //  (present if flags[6] is set)
        u32 mmap_addr;   //   (present if flags[6] is set)
        u32 drives_length; // (present if flags[7] is set)
        u32 drives_addr; //   (present if flags[7] is set)
        u32 config_table; //  (present if flags[8] is set)
        u32 boot_loader_name; // (present if flags[9] is set)
        u32 apm_table; //    (present if flags[10] is set)
        u32 vbe_control_info; // (present if flags[11] is set)
        u32 vbe_mode_info; 
        u16 vbe_mode;
        u16 vbe_interface_seg;
        u16 vbe_interface_off;
        u16 vbe_interface_len;
} __attribute__((packed));


// this is the format of a single node corresponding to a
// homogeneous region of address space. this is the format
// it which it is given to us by the multiboot compliant
// bootloader. 
struct bios_addr_map
{
	u32 node_size; // in bytes
	u32 base_addr_low;
	u32 base_addr_high;
	u32 length_low;
	u32 length_high;
	u32 type;
} __attribute__((packed));

struct bios_drive_info
{
	u32 size;
	u8 drive_no;
	u8 drive_mode;
	u16 drive_cylinders;
	u8 drive_heads;
	u8 drive_sectors;
	u16 *start_port_array;
} __attribute__((packed));



enum addr_type
	{
		RAM
	};



void read_multiboot_information (u32 *multiboot_info_ptr);
void display_boot_progress (const char *message, bool result);
char* columnlize_string (const char *istr, char *ostr, int screen_width, 
			 int left_margin, int right_margin, int fillbyte);


const char* get_bios_addr_buffer (int *size);
#endif // SYSINFO_H_




