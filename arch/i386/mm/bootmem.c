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
#include <string.h>

typedef struct {
	uint start_bit;
	uint end_bit;
}bm_account;

#define TOTAL_ACNT_NODES 1000

static uint *start_addr_acnt = NULL;


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

static void bm_setbit (uint bitno, int init_area);
static void bm_clearbit (uint bitno, int init_area);
static void bm_setbit_range (uint start, uint end, int init_area);
static uint bm_getbit (uint bitno);
static int find_first_free (uint *bit, uint from);
static int find_free_bits_range (uint total_bits, uint *start_bit);
static int find_free_bits_range_from (uint total_bits, uint start_bit);


extern char __start_, __end_;

static uint bm_heap_start;
static const uint bm_heap_size = 8192; //16777216; 
static uint bm_heap_end;



#define GETBIT(a) (uint)(a)/PAGE_SIZE
#define FROMBIT(b) (uint)(b*PAGE_SIZE)
	


/** \brief Initialize the boot memory allocator
 *
 * This takes RAM information from the multiboot header and then sets up some
 * data structures to manage this RAM
 */
int init_bootmem_allocator (void)
{
	const struct bios_addr_map *s;
	int buf_size;
	int r;
	uint start_addr;
	uint end_addr;
	//uint end_bit = 0;

	s = (const struct bios_addr_map*)get_bios_addr_buffer (&buf_size);
	printf ("Multiboot buffer at address 0x%8x\n", (uint)s);

	int node_total_size; // = s->node_size +
			     // sizeof(s->node_size);

	for (;buf_size; buf_size -= node_total_size){
		start_addr = s->base_addr_low; // points to
					       // first word 
		_ASSERT_DEBUG((s->length_low), EMultibootDataCorrupt, 
			      (uint)start_addr, (uint)s->length_low, 0);
		end_addr = (uint)start_addr + (uint)s->length_low - 1; // points to
		                                             // last word
		printf ("Block: start_addr = %8x end_addr = %8x type = %d\n", 
			start_addr, end_addr, s->type);
		
		if (s->type == MULTIBOOT_RAM){
			printf (".. ram ..");
			_ASSERT_DEBUG(!(start & 0xFFF), EMultibootRAMAddrBad, (uint)start, (uint)end, 0);
			if ((uint)&__start_ <= end){
				_ASSERT_DEBUG(((uint)&__end_ <= end),EMultibootRAMAddrBad,(uint)end,(uint)&__start_,(uint)&__end_);
				printf (". set-ganoid .\n");
ILP
				bm_setbit_range (GETBIT(&__start_), GETBIT(&__end_));
			}
			printf (".. all free .\n");
		}
		else{
			printf (".. reserved ..");
			printf (". set .\n");
			delay (18000000);
			bm_setbit_range (GETBIT(start), GETBIT(end));
			}
		//printf ("\n");
		delay (2000000);
		node_total_size = s->node_size + sizeof(s->node_size);
		s = (const struct bios_addr_map*)((uint)s + node_total_size);
	}

	bm_heap_start = ROUND_TO_PAGE_SIZE(&__end_);
	_ASSERT_DEBUG(((MAX_UINT-bm_heap_start) >= bm_heap_size) ,EBootMemHeapOverFlow, (uint)bm_heap_start, (uint)bm_heap_size,(uint)&__end_);

	uint heap_start_bit = GETBIT(bm_heap_start);
	uint total_heap_req_bits = bm_heap_size/PAGE_SIZE;
	bm_heap_end = bm_heap_start + bm_heap_size;

	// just check if the free bits are really there. A failure would mean we do not have
	// that much RAM in the AREA requested as heap.
	r = find_free_bits_range_from(total_heap_req_bits, heap_start_bit);
	_ASSERT_DEBUG((0 == r),EBootMemHeapOverFlow,bm_heap_start,bm_heap_size,(uint)&__end_);
	
	
	printf ("__end_ = %8x\nbm_heap_start = %8x\nbm_heap_size = %8x\nbm_heap_end = %8x\n",
	&__end_, bm_heap_start, bm_heap_size, bm_heap_end);



	{
		// initialize account area
		uint asize = ( sizeof (bm_account) * TOTAL_ACNT_NODES );
		uint abits_req = asize/PAGE_SIZE + 1;
		printf ("abits_req = %d\n", abits_req);
		uint start_bit;
		uint end_bit;
		r = find_free_bits_range (abits_req, &start_bit);
		_ASSERT((0 == r), EBootMemNoMemory, bm_heap_start, bm_heap_size, asize);
		end_bit = start_bit + abits_req - 1;
		bm_setbit_range (start_bit, end_bit, 0);
		start_addr_acnt = (uint*)FROMBIT(start_bit);
		printf ("Accounting area for bootmemory allocator set up between\n"
			"0x%8x and 0x%8x\n", start_addr_acnt, FROMBIT(end_bit + 1)-1);
	}

	return 0;
}


static void bm_setbit_range (uint start, uint end, int init_area)
{
	printf ("setbit range: %u to %u\n", start, end);
	delay (8000000);
	for (uint i = start; i <= end; i++) bm_setbit (i, init_area);
}


static void bm_clearbit_range (uint start, uint end, int init_area)
{
	for (uint i = start; i <= end; i++) bm_clearbit (i, init_area);
}



static void bm_setbit (uint bitno, int init_area)
{
	int index = (bitno/32);
	int offset = (bitno%32);
	uint mask = 1 << offset;
	//printf ("setting bit %d\n .. free_bitmap[index] = 0x%8x\n", bitno, free_bitmap[index]);
	free_bitmap[index] |= mask;
	printf ("free_bitmap[%d] = 0x%8x Start_Addr = 0x%8x\n",index, free_bitmap[index],
		FROMBIT(bitno));
	delay (8000000);
#ifdef GANOID_DEBUG
	if (init_area)
		memset ((void*)FROMBIT(bitno), 0, PAGE_SIZE);
#endif	
}

static void bm_clearbit (uint bitno, int init_area)
{
	int index = (bitno/32);
	int offset = (bitno%32);
	uint mask = 1 << offset;
	free_bitmap[index] &= ~mask;
#ifdef GANOID_DEBUG
	if (init_area)
		memset ((void*)FROMBIT(bitno), (int)0xDEADBEEF, PAGE_SIZE);
#endif	
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
	uint mask = 1 << offset;
	bool free_bit_found = 0;

	if (start_bit > end_bit)
		return 1;

	//	uint local_start_bit = offset;
	//printf ("%s\n", __FUNCTION__);
	uint i;
	for (i = start_bit; i < end_bit; index++){
		for (; mask; mask = mask<<1, i++) {
			if (!(free_bitmap[index] & mask)){
				free_bit_found = 1;
				break;
			}
		}
		if (free_bit_found)
			break;
		mask = 0x00000001;
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
	bool success = 0;

	/*
	//printf ("%s\n", __FUNCTION__);
	//printf ("l_start_bit: 0x%8x\ntotal_bits: 0x%8x\nend_bit: 0x%8x\n", FROMBIT(l_start_bit), FROMBIT(total_bits), FROMBIT(end_bit));
	*/


	uint i;
	uint b;
	uint remaining_bits = total_bits - 1;
	for (;(l_start_bit + total_bits) <= end_bit;){
		//printf ("Checking from bit %d ... ", l_start_bit);
		if (0 == find_first_free(&first_free_bit, l_start_bit)){
			// we already have one bit free. now
			// we need to check for remaining
			// bits in this range
			//printf ("first free = %d .... others ...", first_free_bit);
			for (b = first_free_bit + 1, i = 0; i < remaining_bits; i++, b++){ 
				//printf (".%d.", b);
				if (bm_getbit (b))
					break;
			}
			if (i == remaining_bits){
				//printf ("..Ok\n");
				success = 1;
				break;
			}
		}
		//printf ("Fail\n");
		l_start_bit = first_free_bit+i;
	}
	//printf ("done\n");
	if (success){
		*start_bit = first_free_bit;
		return 0;
	}
	else
		return 1;
}

static int find_free_bits_range_from (uint total_bits, uint start_bit)
{
	uint first_free_bit;
	uint l_start_bit = start_bit;
	uint end_bit = GETBIT(bm_heap_end);
	bool success = 0;

	/*
	//printf ("Heap size requested = 0x%x bytes, starting from address = 0x%x,"
		"upto address 0x%x\n", FROMBIT(total_bits), FROMBIT(start_bit), 
		FROMBIT(end_bit));*/


	uint i;
	if ((l_start_bit + total_bits) <= end_bit){
		if (0 == find_first_free(&first_free_bit, l_start_bit)){
			if (first_free_bit == l_start_bit){
				for (i = 1; i < total_bits; i++){
					if (bm_getbit (first_free_bit+i))
						break;
				}
				if (i == total_bits)
					success = 1;
				
			}
		}
		
	}
	if (success)
		return 0;
	else
		return 1;
}



void* bm_malloc (uint size)
{
	uint r;
	//size = 17000;
 	uint rsize = ROUND_TO_PAGE_SIZE(size);
	uint bits = rsize/PAGE_SIZE;
	uint start_bit;
	//printf ("bits = %d\n", bits);
	r = find_free_bits_range (bits, &start_bit);
	//printf ("r = %d\n", r);
	if (r)
		return NULL;

	uint end_bit = start_bit + bits - 1;

	bm_setbit_range (start_bit, end_bit, 1);

	return (void*)FROMBIT(start_bit);;
}


void* bm_free (void *ptr)
{
	uint r;
	//size = 17000;
 	uint rptr = ROUND_TO_PAGE_SIZE(ptr);
	printf ("ptr = 0x%8x\n");
	if (rptr > (uint)ptr)
		rptr = (uint)rptr - PAGE_SIZE;

	printf ("rptr = 0x%8x\n");
	/*
	uint bits = rsize/PAGE_SIZE;
	uint start_bit;
	//printf ("bits = %d\n", bits);
	r = find_free_bits_range (bits, &start_bit);
	//printf ("r = %d\n", r);
	if (r)
		return NULL;

	uint end_bit = start_bit + bits - 1;

	bm_setbit_range (start_bit, end_bit);

	return (void*)FROMBIT(start_bit);*/
}


 
