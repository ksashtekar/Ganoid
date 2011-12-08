#ifndef __PAGE_H__
#define __PAGE_H__

#include <mm.h>

#define GETBIT(a) (u32)(a)/PAGE_SIZE
#define FROMBIT(b) (u32)(b*PAGE_SIZE)

void init_page_allocator(void);
void* get_free_pages(unsigned count);
void free_page(void *ptr);

#endif // __PAGE_H__
