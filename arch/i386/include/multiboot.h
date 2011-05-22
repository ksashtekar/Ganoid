#ifndef _MULTIBOOT_H
#define _MULTIBOOT_H

#ifdef HAVE_ASM_USCORE
#define EXT_C(sym) _ ## sym
#else
#define EXT_C(sym) sym
#endif


#define MULTIBOOT_HEADER_MAGIC 0x1BADB002	


#define FLAG_BOOT_MODULES_4K_ALIGNED 0x01 << 0
#define FLAG_MEMINFO 0x01 << 1
#define FLAG_VIDEO_MODE_TABLE 0x01 << 2
#define FLAG_MANUAL_LOAD_ADDR 0x01 << 16
// commented out this line because the new grub2 on my ubuntu does not know about it yet ...
//#define MULTIBOOT_HEADER_FLAGS (FLAG_VIDEO_MODE_TABLE | FLAG_MEMINFO | FLAG_BOOT_MODULES_4K_ALIGNED)
#define MULTIBOOT_HEADER_FLAGS (FLAG_MEMINFO | FLAG_BOOT_MODULES_4K_ALIGNED)

#define STACK_SIZE 0x4000


#endif // _MULTIBOOT_H
