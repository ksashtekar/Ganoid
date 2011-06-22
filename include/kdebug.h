#ifndef KDEBUG_H_
#define KDEBUG_H_

#include <ganoid/types.h>



#define  KDEBUG_INIT_VAL  0xCAFED00D



enum {
	EMultibootRAMAddrBad = 0,
	EBootMemHeapOverFlow = 1,
	EBootMemNoMemory = 2,
	EMultibootDataCorrupt = 3,
	EMultiBootSpaceInsufficient = 4,
	EMultibootFreeAddrInvalid = 5,
	EBootmemAllocatorNotInitialized = 6,
	EDispatcherNotInitialized = 7,
	ENullPointer = 8
};


enum kdebug_dump_format_width
	{
		EDUMP_BYTE,
		EDUMP_HALFWORD,
		EDUMP_WORD,
	};

enum kdebug_dump_format_type
	{
		EDUMP_DECIMAL,
		EDUMP_HEX,
	};

void kernel_fault (const char *str)  __attribute__((noreturn));
void data_dump (void *start_addr, void *end_addr, 
		enum kdebug_dump_format_width width, 
		enum kdebug_dump_format_type type);

#define DUMP_HALFWORD(a) data_dump_halfword (a)
#define DUMP_WORD(a) data_dump_word (a)

void data_dump_byte (void *start_addr);
void data_dump_word (void *start_addr);
void data_dump_halfword (void *start_addr);

void display_error_info (const char *cond, int e, 
			 const char* n_v1, u32 v1, 
			 const char* n_v2, u32 v2, 
			 const char* n_v3, u32 v3,
			 const char *file_name, u32 line_no);


#define DUMP_REGISTERS					\
	{						\
	int reg;					\
	asm volatile ("mov %%eax, %0" : : "m"(reg));    \
	printf ("EAX : 0x%8x\t", reg);			\
	asm volatile ("mov %%ebx, %0" : : "m"(reg));    \
	printf ("EBX : 0x%8x\n", reg);			\
	asm volatile ("mov %%ecx, %0" : : "m"(reg));    \
	printf ("ECX : 0x%8x\t", reg);			\
	asm volatile ("mov %%edx, %0" : : "m"(reg));    \
	printf ("EDX : 0x%8x\n", reg);			\
	asm volatile ("mov %%esi, %0" : : "m"(reg));    \
	printf ("ESI : 0x%8x\t", reg);			\
	asm volatile ("mov %%edi, %0" : : "m"(reg));    \
	printf ("EDI : 0x%8x\n", reg);			\
	asm volatile ("mov %%ebp, %0" : : "m"(reg));    \
	printf ("EBP : 0x%8x\t", reg);			\
	asm volatile ("mov %%esp, %0" : : "m"(reg));    \
	printf ("ESP : 0x%8x\n", reg);			\
        asm volatile ("mov %%cs, %0" : : "m"(reg));	\
	printf ("CS  : 0x%8x\t", reg);			\
	asm volatile ("mov %%ds, %0" : : "m"(reg));	\
	printf ("DS  : 0x%8x\n", reg);			\
	asm volatile ("mov %%ss, %0" : : "m"(reg));	\
	printf ("SS  : 0x%8x\t", reg);			\
	asm volatile ("mov %%es, %0" : : "m"(reg));	\
	printf ("ES  : 0x%8x\n", reg);			\
	asm volatile ("mov %%fs, %0" : : "m"(reg));	\
	printf ("FS  : 0x%8x\t", reg);			\
	asm volatile ("mov %%gs, %0" : : "m"(reg));	\
	printf ("GS  : 0x%8x\n", reg);			\
	asm volatile ("1:");				\
	asm volatile ("jmp 1b");			\
	}


#define _ASSERT_DEBUG(c,e,v1,v2,v3)		\
	if (!c){							\
		display_error_info (#c, e, #v1, v1, #v2, v2, #v3, v3, __FILE__, __LINE__); \
		while (1){}						\
	} 

#define _ASSERT(c,e,v1,v2,v3)		\
	if (!c){							\
		display_error_info (#c, e, #v1, v1, #v2, v2, #v3, v3, __FILE__, __LINE__); \
		while (1){}						\
	} 

#endif // KDEBUG_H_
