#ifndef _MULTIBOOT_H
#define _MULTIBOOT_H

#ifdef HAVE_ASM_USCORE
#define EXT_C(sym) _ ## sym
#else
#define EXT_C(sym) sym
#endif



typedef unsigned char		multiboot_uint8_t;
typedef unsigned short		multiboot_uint16_t;
typedef unsigned int		multiboot_uint32_t;
typedef unsigned long long	multiboot_uint64_t;

struct multiboot_header
{
  /* Must be MULTIBOOT_MAGIC - see above.  */
  multiboot_uint32_t magic;

  /* Feature flags.  */
  multiboot_uint32_t flags;

  /* The above fields plus this one must equal 0 mod 2^32. */
  multiboot_uint32_t checksum;

  /* These are only valid if MULTIBOOT_AOUT_KLUDGE is set.  */
  multiboot_uint32_t header_addr;
  multiboot_uint32_t load_addr;
  multiboot_uint32_t load_end_addr;
  multiboot_uint32_t bss_end_addr;
  multiboot_uint32_t entry_addr;

  /* These are only valid if MULTIBOOT_VIDEO_MODE is set.  */
  multiboot_uint32_t mode_type;
  multiboot_uint32_t width;
  multiboot_uint32_t height;
  multiboot_uint32_t depth;
};



#define GRUB_V_0_99



#define MULTIBOOT_HEADER_MAGIC 0x1BADB002	


#define FLAG_BOOT_MODULES_4K_ALIGNED 0x01 << 0
#define FLAG_MEMINFO 0x01 << 1
#define FLAG_VIDEO_MODE_TABLE 0x01 << 2
#define FLAG_MANUAL_LOAD_ADDR 0x01 << 16
// commented out this line because the new grub2 on my ubuntu does not know about it yet ...
//#define MULTIBOOT_HEADER_FLAGS (FLAG_VIDEO_MODE_TABLE | FLAG_MEMINFO | FLAG_BOOT_MODULES_4K_ALIGNED)
#define MULTIBOOT_HEADER_FLAGS (FLAG_MEMINFO | FLAG_BOOT_MODULES_4K_ALIGNED)

/* is there basic lower/upper memory information? */
#define MULTIBOOT_INFO_MEMORY			0x00000001
/* is there a boot device set? */
#define MULTIBOOT_INFO_BOOTDEV			0x00000002
/* is the command-line defined? */
#define MULTIBOOT_INFO_CMDLINE			0x00000004
/* are there modules to do something with? */
#define MULTIBOOT_INFO_MODS			0x00000008

/* These next two are mutually exclusive */

/* is there a symbol table loaded? */
#define MULTIBOOT_INFO_AOUT_SYMS		0x00000010

/* is there a full memory map? */
#define MULTIBOOT_INFO_MEM_MAP			0x00000020

/* Is there drive info?  */
#define MULTIBOOT_INFO_DRIVE_INFO		0x00000080

/* Is there a config table?  */
#define MULTIBOOT_INFO_CONFIG_TABLE		0x00000100

/* Is there a boot loader name?  */
#define MULTIBOOT_INFO_BOOT_LOADER_NAME		0x00000200

/* Is there a APM table?  */
#define MULTIBOOT_INFO_APM_TABLE		0x00000400

/* Is there video information?  */
#define MULTIBOOT_INFO_VBE_INFO		        0x00000800
#define MULTIBOOT_INFO_FRAMEBUFFER_INFO	        0x00001000

#define STACK_SIZE 0x4000



#endif // _MULTIBOOT_H
