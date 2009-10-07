#ifndef KDEBUG_H_
#define KDEBUG_H_

#include <ktypes.h>

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

void kernel_fault (const char *str);
void data_dump (void *start_addr, void *end_addr, 
		enum kdebug_dump_format_width width, 
		enum kdebug_dump_format_type type);

#endif // KDEBUG_H_
