
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
#include <mm.h>


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

static void bm_setbit (uint bitno);
static void bm_clearbit (uint bitno);
static void bm_setbit_range (uint start, uint end);
static uint bm_getbit (uint bitno);
static int find_first_free (uint *bit, uint from) 



extern char __start_, __end_;

static uint bm_heap_start;
const static uint bm_heap_size = 16777216; 
static uint bm_heap_end;



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
	//uint end_bit = 0;

	bm_heap_start = ROUND_TO_PAGE_SIZE(&__end_);
	bm_heap_end = bm_heap_start + bm_heap_size;
	printf ("__end_ = %8x\nbm_heap_start = %8x\nbm_heap_size = %8x\nbm_heap_end = %8x",
		&__end_, bm_heap_start, bm_heap_size, bm_heap_end);

	int node_total_size; // = s->node_size + sizeof(s->node_size);

	for (;buf_size; buf_size -= node_total_size)   {
		start = s->base_addr_low;
		end = start + s->length_low;
		printf ("start = %8x end = %8x ", start, end);
		if (s->type == MULTIBOOT_RAM){
			printf (".. ram ..");
			_ASSERT_DEBUG(!(start & 0xFFF), EMultibootRAMAddrBad, start,end,start_bit);
			if ((uint)&__start_ <= end){
				_ASSERT_DEBUG(((uint)&__end_ <= end),EMultibootRAMAddrBad,end,(uint)&__start_,(uint)&__end_);
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
		s = (const struct bios_addr_map*)((uint)s + node_total_size);
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



static void bm_setbit (uint bitno)
{
	int index = (bitno/32);
	int offset = (bitno%32);
	uint mask = 1 << offset;
	free_bitmap[index] |= mask;
}

static void bm_clearbit (uint bitno)
{
	int index = (bitno/32);
	int offset = (bitno%32);
	uint mask = 1 << offset;
	free_bitmap[index] &= ~mask;
}

static uint bm_getbit (uint bitno)
{
	int index = (bitno/32);
	int offset = (bitno%32);
	uint mask = 1 << offset;
	return (free_bitmap[index] & mask);
}

static int find_first_free (uint *bit, uint from) 
{
	uint start_bit  = from;
	uint end_bit = GETBIT(bm_heap_end);
	int index = (start_bit/32);
	int offset = (start_bit%32);
	int bit_gap = 32 - offset;
	uint mask = 1 << offset;
	bool free_bit_found = false;
	//	uint local_start_bit = offset;
	for (uint i = start_bit; i  < end_bit;index++){
		if (free_bitmap[index]){
			for (;mask;mask>>1, i++)
				if (!(free_bitmap[index] & mask)){
					free_bit_found = true;
					break;
				}
		}
		if (free_bit_found)
			break;
		mask = 0x80000000;
	}
	if (free_bit_found){
		*bit = i;
		return 0;
	}
	else
		return 1;
}


static int find_free_bits_range (uint total_bits, uint *start_bit)
{
	uint first_free_bit;
	uint l_start_bit = GETBIT(bm_heap_start);
	uint end_bit = GETBIT(bm_heap_end);
	bool success = false;

	for (;(l_start_bit + total_bits) < end_bit;){
		if (find_first_free(&first_free_bit, l_start_bit))
			return NULL;
		for (uint i = 1; i < total_bits; i++){
			if (!bm_getbit (first_free_bit+i))
				break;
		}
		if (i == total_bits){
			sucess = 1;
			break;
		}
		l_start_bit = first_free_bit+i;
	}
	if (success){
		*start_bit = l_start_bit;
		return 0;
	}
	else
		return 1;
}



void* bm_malloc (uint size)
{
	uint rsize = ROUND_TO_PAGE_SIZE(size);
	
}

