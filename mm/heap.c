/**
 * kmalloc implementation
 *
 * This is the worst possible implementation of malloc. It allocates
 * contiguous page chain for every kmalloc and _never_ frees it. Its a
 * stopgap implementation before a good one comes by.
 */

#include <kernel.h>
#include <mm.h>
#include <mm/page.h>
#include <mm/heap.h>


void *kmalloc(unsigned size)
{
	unsigned page_cnt = ((size / PAGE_SIZE) + 1);
	printk("page_cnt: %d\n", page_cnt);
	return get_free_pages(page_cnt);
}

void kfree(void *ptr __attribute__((unused)))
{
	/* Do nothing */
}
