/*
 *  mm/slab.c
 *
 *  Slab allocator:
 *       Accepts a statically/dynamically allocated memory area and the
 *       allocation element size. Future allocations/deallocations will be
 *       done from this area. If allowed (specified by a flag during init)
 *       then out of memory on the pre-allocated area will be handled
 *       by dynamically allocating a physical page.
 *
 *  Copyright (C) 2011-2022 Kaustubh Ashtekar
 *
 */

#include <ganoid/types.h>


typedef struct {
    void *bitmap;
    void *first_element;
    void *next_block;
    u32 flags; /* 
		  0: Dynamic expansion allowed
		  1-15: Reserved
		  16-31: Allocation alignment
		*/
}slab_db_t;





handle _slab_create(void *block_addr, unsigned element_size, 
		   bool allow_dynamic, unsigned alignment_required)
{
    return;
}



handle slab_create(void *block_addr, unsigned element_size, 
		   bool allow_dynamic, unsigned alignment_required)
{
    if(!block_addr)
	return 0;
    
    return _slab_create(block_addr, element_size, 
		   allow_dynamic, alignment_required);
}




handle slab_create(void *block_addr, unsigned element_size)
{
    return slab_create(block_addr, element_size, FALSE, 4);
}



handle slab_create_dynamic(void *block_addr, unsigned element_size)
{
    return slab_create(block_addr, element_size, TRUE, 4);
}


handle slab_create_aligned(void *block_addr, unsigned element_size,
			   unsigned alignment)
{
    return slab_create(block_addr, element_size, FALSE, alignment);
}
