#ifndef _BOOT_MEM_H
#define _BOOT_MEM_H


int init_bootmem_allocator (void);
void* bm_malloc (uint size);
void* bm_free (void *ptr);





#endif // _BOOT_MEM_H
