#ifndef __SYSINFO_H__
#define __SYSINFO_H__

#include <ganoid/types.h>
#include <multiboot.h>

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

struct bios_drive_info {
	u32 size;
	u8 drive_no;
	u8 drive_mode;
	u16 drive_cylinders;
	u8 drive_heads;
	u8 drive_sectors;
	u16 *start_port_array;
} __attribute__ ((packed));

typedef struct {
	unsigned saddr;		/* start address */
	unsigned len;
} ram_map_t;

void read_multiboot_information(u32 * multiboot_info_ptr);
void display_boot_progress(const char *message, bool result);
char *columnlize_string(const char *istr, char *ostr,
			int screen_width, int left_margin,
			int right_margin, char fillbyte);

const struct multiboot_mmap_entry *get_bios_addr_buffer(unsigned *node_cnt);
const ram_map_t *get_rammap_ptr(int *rammap_nodes);
#endif // __SYSINFO_H__
