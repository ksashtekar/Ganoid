/*
 * mm/page.c
 *
 * Physical page(s) allocator.
 *
 * Returns one or more physically contiguous pages. 
 * Marks them as busy
 *
 * Copyright (C) 2011 Kaustubh Ashtekar
 */


#define ADDR_START_MAGIC 0x100000

/* FIXME: To simplify in the first step
 only the RAM from the address ADDR_START_MAGIC will
be used. This is the start address of the area to be reported a
as the biggest contiguous area available*/

/*
 * - Get info from sysinfo module about the ram map
 */
void init_free_page_alloctor()
{
    u32 node_cnt;
    const ram_map_t* n = get_rammap_ptr(&node_cnt);

    _ASSERT(node_cnt);

    // search for ADDR_START_MAGIC    
    int i;
    for(i = 0; i < node_cnt; i++)
	if(n[i]->start_addr == ADDR_START_MAGIC)
	    break;

    _ASSERT(n[i]->start_addr == ADDR_START_MAGIC);
    
    
}


void *get_free_pages(int count)
{
    
}


void *get_free_page()
{
    get_free_pages(1);
}





int init_page_allocator()
{
    
}

