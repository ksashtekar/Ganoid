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

#include <ganoid/types.h>
#include <kernel.h>
#include <sysinfo.h>
#include <bootmem.h>
#include <kdebug.h>
#include <mm.h>
#include <mm/page.h>
#include <string.h>
#include <utils.h>

/*
 * In a 32 bit system there are 1048576 pages in the address space.
 * Not all of them are usable as RAM as some of them are mapped 
 * to other hardware devices. Hence, we need to get information
 * from the hardware and allocate only pages which map to RAM.
 *
 * 1048576 pages => 1048576 bits.
 * 
 * Only the bits which represent RAM pages are used. Others 
 * are don't care.
 *
 * We do not keep any accounting of the range of pages
 * allocated. We do not know the count of pages allocated in one
 * function call. So, we do not have a function which can free many
 * pages at once. The user has to free each page indivisually.
 */

#define DEBUG_PAGE_ALLOC
#ifdef DEBUG_PAGE_ALLOC
#define MDEBUG printk
#else /* ! DEBUG_PAGE_ALLOC */
#define MDEBUG(...)
#endif /* DEBUG_PAGE_ALLOC */

#define GET_PAGENO(a) (unsigned)((unsigned)a)/PAGE_SIZE

#define PAGENO_TO_ADDR(b) (unsigned)(b*PAGE_SIZE)
#define NUM_BITS_PER_UINT (sizeof(unsigned)*8)
#define LAST_BIT_NO (unsigned)(NUM_FRAMES - 1)

#define UINT_CNT NUM_FRAMES/NUM_BITS_PER_UINT
static unsigned free_bm[UINT_CNT];

// ganoid linker script variables.
extern char __start_, __end_;

#define GANOID_AREA_START (unsigned)&__start_
#define GANOID_AREA_END (unsigned)&__end_

static void fp_setbit(unsigned b)
{
	unsigned index = (b / NUM_BITS_PER_UINT);
	unsigned offset = (b % NUM_BITS_PER_UINT);
	unsigned mask = (unsigned)(1 << offset);
	free_bm[index] |= mask;
}

static void fp_clearbit(unsigned b)
{
	unsigned index = (b / NUM_BITS_PER_UINT);
	unsigned offset = (b % NUM_BITS_PER_UINT);
	unsigned mask = (unsigned)(1 << offset);
	free_bm[index] &= ~mask;
}

static bool fp_getbit(unsigned b)
{
	unsigned index = (b / NUM_BITS_PER_UINT);
	unsigned offset = (b % NUM_BITS_PER_UINT);
	unsigned mask = (unsigned)(1 << offset);
	return (free_bm[index] & mask) && (free_bm[index] & mask);
}

/**
 * @todo Optimize
 */
static void fp_setbit_range(unsigned start_bit, unsigned end_bit)
{
	unsigned bit = start_bit;
	for (; bit <= end_bit; bit++)
		fp_setbit(bit);
}

/**
 * @todo Optimize
 */
static void fp_clearbit_range(unsigned start_bit, unsigned end_bit)
{
	MDEBUG("%s: %d:%x %d:%x\n", __FUNCTION__,
	       start_bit, PAGENO_TO_ADDR(start_bit), end_bit,
	       PAGENO_TO_ADDR(end_bit));
	unsigned bit = start_bit;
	for (; bit <= end_bit; bit++)
		fp_clearbit(bit);
}

/*
 * Assuming 'from' will never be zero.
 * @return On success 0 is returned and bit points to the 
 * first free bit found. On error, 1 is returned.
 */
static unsigned find_first_free(unsigned *bit, unsigned from)
{
	unsigned index = (from / NUM_BITS_PER_UINT);
	const unsigned offset = (from % NUM_BITS_PER_UINT);
	unsigned mask = (unsigned)(1 << offset);
	bool free_bit_found = FALSE;

	unsigned i;
	for (i = from; i; index++, mask = 0x1) {
		for (; mask; mask = mask << 1, i++) {
			if (!(free_bm[index] & mask)) {
				free_bit_found = TRUE;
				break;
			}
		}
		if (free_bit_found)
			break;
	}
	if (free_bit_found) {
		*bit = i;
		return 0;
	} else
		return 1;
}

/**
 * Find a range of free bits. This is useful for allocating
 * a set of contiguous pages.
 *
 * @todo Optimize
 *
 * @return On success 0 is returned and \a start_bit is set
 * to appropriate value. On error 1 is returned.
 */
static int find_free_bits_range(unsigned count, unsigned *start_bit)
{
	unsigned first_free_bit;
	unsigned l_start_bit = 0;
	bool success = FALSE;

	unsigned i, b;
	unsigned remaining_bits = count - 1;
	for (; (l_start_bit + remaining_bits) <= LAST_BIT_NO;) {
		if (0 == find_first_free(&first_free_bit, l_start_bit)) {
			/* Now check whether the following
			   'remaining_bits' are also free */
			for (b = first_free_bit + 1, i = 0;
			     (i < remaining_bits); i++, b++) {
				if (fp_getbit(b))
					break;
			}
			if (i == remaining_bits) {
				success = TRUE;
				break;
			}
		} else
			break;

		l_start_bit = first_free_bit + i;
	}
	if (success) {
		*start_bit = first_free_bit;
		return 0;
	} else
		return 1;
}

static void dump_free_info(void) __attribute__ ((unused));
static void dump_free_info(void)
{
	unsigned bit;

	MDEBUG("Free/Alloc map\n\n");
	unsigned start = 0;
	bool init_value = fp_getbit(start);
	for (bit = 1; bit <= LAST_BIT_NO; bit++) {
		if ((fp_getbit(bit) != init_value) || (bit == LAST_BIT_NO)) {
			MDEBUG("0x%x to 0x%x :: ",
			       PAGENO_TO_ADDR(start), PAGENO_TO_ADDR(bit) - 1);
			if (init_value)
				MDEBUG("Allocated. Size: ");
			else
				MDEBUG("Free. Size: ");
			print_human_readable_size(PAGENO_TO_ADDR(bit) -
						  PAGENO_TO_ADDR(start));
			start = bit;
			init_value = fp_getbit(start);
		}
	}
	MDEBUG("Done\n");
	while (1) {
	}
}

/**
 * Initialize the physical page allocator.
 *
 * This takes RAM information from the multiboot header 
 * and then sets up some data structures to manage this RAM.
 */
void init_page_allocator()
{
	/* set all bits to busy .. we will clear bits which actually
	   point to usable RAM */
	for (unsigned x = 0; x < UINT_CNT; x++)
		free_bm[x] = (unsigned)~0;

	unsigned node_cnt;
	const ram_map_t *s = get_rammap_ptr(&node_cnt);
	printk("RAM address map received from multiboot:\n");
	for (unsigned i = 0; i < node_cnt; i++) {
		printk("%d: Start: 0x%8x End:0x%8x :: Size = ",
		       i, s[i].saddr, s[i].saddr + s[i].len - 1);
		print_human_readable_size(s[i].len);
	}

	printk("Area occupied by Ganoid:\n");
	printk("Start: 0x%8x\n", GANOID_AREA_START);
	printk("End  : 0x%8x\n", GANOID_AREA_END);

	/* set the bits which are already occupied. */
	for (unsigned i = 0; i < node_cnt; i++) {
		unsigned next_block_addr = s[i].saddr + s[i].len;
		if (s[i].saddr <= GANOID_AREA_START &&
		    GANOID_AREA_END < next_block_addr) {
			/* kernel is inside current RAM area */
			MDEBUG("This area has ganoid\n");

			/* free pages upto kernel start */
			if (s[i].saddr != GANOID_AREA_START)
				fp_clearbit_range(GET_PAGENO(s[i].saddr),
						  GET_PAGENO(GANOID_AREA_START -
							     1));

			if (next_block_addr != GANOID_AREA_END) {
				MDEBUG("SEQ 0: %x %x\n", GANOID_AREA_END +
				       PAGE_SIZE, next_block_addr - 1);
				fp_clearbit_range(GET_PAGENO(GANOID_AREA_END +
							     PAGE_SIZE),
						  GET_PAGENO(next_block_addr -
							     1));
			}
		} else {
			MDEBUG("\nFull free range: %x to %x\n",
			       s[i].saddr, next_block_addr - 1);
			fp_clearbit_range(GET_PAGENO(s[i].saddr),
					  GET_PAGENO(next_block_addr - 1));
		}
	}
	//    dump_free_info();    
}

void *get_free_pages(unsigned count)
{
	unsigned start_bit;

	int r = find_free_bits_range(count, &start_bit);
	if (r != 0)
		return NULL;

	unsigned last_bit_no = start_bit + count - 1;
	fp_setbit_range(start_bit, last_bit_no);

	return (void *)PAGENO_TO_ADDR(start_bit);;
}

void free_page(void *ptr)
{
	unsigned bit = GET_PAGENO(ptr);
	if (ptr)
		fp_clearbit(bit);
}
