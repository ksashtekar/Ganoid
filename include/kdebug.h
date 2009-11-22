#ifndef KDEBUG_H_
#define KDEBUG_H_

#include <ktypes.h>








enum {
	EMultibootRAMAddrBad = 0,
	EBootMemHeapOverFlow = 1,
	EBootMemNoMemory = 2,
	EMultibootDataCorrupt = 3,
	EMultiBootSpaceInsufficient = 4,
	EMultibootFreeAddrInvalid = 5
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
			 const char* n_v1, uint v1, 
			 const char* n_v2, uint v2, 
			 const char* n_v3, uint v3,
			 const char *file_name, uint line_no);


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
