
/***************************************************************************
 * This file implements the bootmem allocator.
 * The basic reason for the existence of bootmem allcator is non-existence 
 * of the kernel memory allocator at kernel startup. Before the full blown 
 * kernel memory allcator is available this allocator will satisfy memory 
 * needs of kernel init code
 *
 *
 ***************************************************************************/

#include <ktypes.h>
#include <kernel.h>
#include <sysinfo.h>
#include <bootmem.h>
#include <kdebug.h>


/*
 * There could be maximum 1048576 pages available. Ofcourse not all of them
 * are usable as RAM as some of them are mapped to other hardware devices.
 * Hence, we need get information from the hardware and accordingly set only 
 * bit which have usable RAM
 * 1048576 pages => 1048576 bits.
 * 
 * One important point to note here is that since we use only one bit for
 * remembering about free pages we cannot distinguish between used pages and
 * non-ram pages just by looking at the array . In the start we mark all non-ram 
 * pages as used. It is the responsibility of the kernel allocator to also consult
 * the system RAM information along with this free/busy array.
 */

#define NO_OF_PAGE_FRAMES 1048576
static uint free_bitmap[NO_OF_PAGE_FRAMES/32] = {0};

static void bm_setbit (int bitno);
static void bm_clearbit (int bitno);
static void bm_setbit_range (uint start, uint end);
extern char __start_, __end_;


#define GETBIT(a) (uint)(a)/4096
	


/** \brief Initialize the boot memory allocator
 *
 * This takes RAM information from the multiboot header and then sets up some
 * data structures to manage this RAM
 */
int init_bootmem_allocator (void)
{
	const struct bios_addr_map *s;
	int buf_size;
	s = (const struct bios_addr_map*)get_bios_addr_buffer (&buf_size);
	uint start;
	uint end = 0;
	uint start_bit = 0;
	uint end_bit = 0;

	int node_total_size; // = s->node_size + sizeof(s->node_size);

	for (;buf_size; buf_size -= node_total_size)   {
		start = s->base_addr_low;
		end = start + s->length_low;
		printf ("start = %8x end = %8x ", start, end);
		if (s->type == MULTIBOOT_RAM){
			printf (".. ram ..");
			_ASSERT_DEBUG(!(start & 0xFFF), EMultibootRAMAddrBad, start,end,start_bit);
			if (& __start_ <= end){
				_ASSERT_DEBUG((&__end_ <= end),EMultibootRAMAddrBad,end,& __start_,& __end_);
				printf ("set-ganoid");
				bm_setbit_range (GETBIT(&__start_), GETBIT(&__end_));
			}
		}
		else{
			printf (".. reserved ..");
			printf ("set");
			bm_setbit_range (GETBIT(start), GETBIT(end));
		}
		printf ("\n");
		delay (2000000);
		node_total_size = s->node_size + sizeof(s->node_size);
		s = (uint)s + node_total_size;
	}

	return 0;
}


static void bm_setbit_range (uint start, uint end)
{
	for (uint i = start; i <= end; i++) bm_setbit (i);
}


static void bm_clearbit_range (uint start, uint end)
{
	for (uint i = start; i <= end; i++) bm_clearbit (i);
}



static void bm_setbit (int bitno)
{
	int index = (bitno/32);
	int offset = (bitno%32);
	uint mask = 1 << offset;
	free_bitmap[index] |= mask;
}

static void bm_clearbit (int bitno)
{
	int index = (bitno/32);
	int offset = (bitno%32);
	uint mask = 1 << offset;
	free_bitmap[index] &= ~mask;
}


/*
void* bm_malloc (uint size)
{}
*/
