#include <ganoid/types.h>

typedef struct {
	u32 start_bit;
	u32 end_bit;
}__attribute__((packed)) bm_account;

#define TOTAL_ACNT_NODES 1000

static bm_account *start_addr_acnt = NULL;
static bm_account *end_addr_acnt = NULL;
static u32 acnt_i = 0; // to store number of allocated
			// account

static int bootmem_init_complete = 0;


/*
 * There could be maximum 1048576 pages available. Of course not all of them
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

static u32 free_bitmap[NO_OF_PAGE_FRAMES/32] = {0};

static void bm_setbit (u32 bitno, int init_area);
static void bm_clearbit (u32 bitno, int init_area);
static void bm_setbit_range (u32 start, u32 end, int init_area);
static u32 bm_getbit (u32 bitno);
static int find_first_free (u32 *bit, u32 from);
static int find_free_bits_range (u32 total_bits, u32 *start_bit);
static int find_free_bits_range_from (u32 total_bits, u32 start_bit);
static bm_account* search_bit_in_acnt (u32 bit);
bm_account* search_free_acnt_node (void);


extern char __start_, __end_;

static u32 bm_heap_start;
static const u32 bm_heap_size = 4096 * 3000; //16777216; 
static u32 bm_heap_end;



#define GETBIT(a) (u32)(a)/PAGE_SIZE
#define FROMBIT(b) (u32)(b*PAGE_SIZE)
	


/** \brief Initialize the boot memory allocator
 *
 * This takes RAM information from the multiboot header and then sets up some
 * data structures to manage this RAM
 */
int init_bootmem_allocator (void)
{
	const ram_map *s = NULL;
	u32 rammap_node_count = kdebug_init_val;
	int r = 0;
	u32 *start_addr = NULL;
	u32 *end_addr = NULL;

	s = get_rammap_ptr (&rammap_node_count);
	printk ("Following information about RAM address map received from multiboot:\n");
	for (u32 i  = 0; i < rammap_node_count; i++)
		printk ("%d: Start: 0x%8x End: 0x%8x\n", i, s[i].start_addr, s[i].end_addr);

	printk ("Area occupied by Ganoid:\n");
	printk ("Start: 0x%8x\n", &__start_);
	printk ("End  : 0x%8x\n", &__end_);


	// set the bits which are already occupied. 
	for (u32 i = 0;i < rammap_node_count; i++){
		start_addr = s[i].start_addr;
		end_addr = s[i].end_addr;
		printk ("Start: 0x%8x End: 0x%8x, ", start_addr, end_addr);
		_ASSERT_DEBUG(!((u32)start_addr & 0xFFF), EMultibootRAMAddrBad,
			      (u32)start_addr, (u32)end_addr, i);
		if ((u32)&__start_  < (u32)end_addr){
			// our code/data is in this block
			_ASSERT_DEBUG(((u32)&__end_ <= (u32)end_addr),EMultibootRAMAddrBad,
				      (u32)end_addr,(u32)&__start_,(u32)&__end_);
				printk (", Ganoid area, ");
				bm_setbit_range (GETBIT(&__start_), GETBIT(&__end_), 0);
		}
		else{
			printk (" free area, ");
			//delay (18000000);
		}
		printk (" Done\n");
		delay (2000000);
	}

	// now initialize the requested heap area.
	printk ("Trying to allocate heap for bootmemory allocator ...\n");
	bm_heap_start = ROUND_TO_PAGE_SIZE(&__end_);
	_ASSERT_DEBUG(((MAX_UINT-bm_heap_start) >= bm_heap_size) ,EBootMemHeapOverFlow, 
		      (u32)bm_heap_start, (u32)bm_heap_size,(u32)&__end_);

	u32 heap_start_bit = GETBIT(bm_heap_start);
	u32 total_heap_req_bits = bm_heap_size/PAGE_SIZE;
	bm_heap_end = bm_heap_start + bm_heap_size - 1;

	// just check if the free bits are really there. A failure would mean we do not have
	// that much RAM in the AREA requested as heap.
	printk ("Searching for free ram in requested area ...\n");
	r = find_free_bits_range_from(total_heap_req_bits, heap_start_bit);
	_ASSERT_DEBUG((0 == r),EBootMemHeapOverFlow,bm_heap_start,
		      bm_heap_size,(u32)&__end_);
	printk ("Ok\n");
	
	printk ("Heap start: 0x%8x\nHeap end: 0x%8x\n", bm_heap_start, bm_heap_end);

	// initialize account area
	u32 asize = ( sizeof (bm_account) * TOTAL_ACNT_NODES );
	u32 abits_req = asize/PAGE_SIZE + 1;
	u32 start_bit = kdebug_init_val;
	u32 end_bit = kdebug_init_val;
	r = find_free_bits_range (abits_req, &start_bit);
	_ASSERT((0 == r), EBootMemNoMemory, bm_heap_start, bm_heap_size, asize);
	end_bit = start_bit + abits_req - 1;
	//printk  ("ddd: %u, %u", start_bit, end_bit);
	//ILP;
	bm_setbit_range (start_bit, end_bit, 0);
	start_addr_acnt = (bm_account*)FROMBIT(start_bit);
	end_addr_acnt = (bm_account*)(FROMBIT(end_bit) + PAGE_SIZE - sizeof(bm_account));
	// zero initialize the account space.
	memset (start_addr_acnt, 0, ((u32)end_addr_acnt - (u32)(start_addr_acnt)));
	printk ("Bootmem allocator accounting area set from 0x%8x to 0x%8x\n", 
		(u32)start_addr_acnt, (u32)end_addr_acnt);

	bootmem_init_complete = 1;
	return r;
}


static void bm_setbit_range (u32 start, u32 end, int init_area)
{
	//printk ("setbit range: %u to %u\n", start, end);
	for (u32 i = start; i <= end; i++) bm_setbit (i, init_area);
}


static void bm_clearbit_range (u32 start, u32 end, int init_area)
{
	for (u32 i = start; i <= end; i++) bm_clearbit (i, init_area);
}



static void bm_setbit (u32 bitno, int init_area)
{
	int index = (bitno/32);
	int offset = (bitno%32);
	u32 mask = 1 << offset;
	//printk ("setting bit %d\n .. free_bitmap[index] = 0x%8x\n", bitno, free_bitmap[index]);
	free_bitmap[index] |= mask;
	/*printk ("Setbit %u in free_bitmap[%d] = 0x%8x Start_Addr = 0x%8x\n",bitno, 
	  index, free_bitmap[index], FROMBIT(bitno));*/
#ifdef GANOID_DEBUG
	if (init_area)
		memset ((void*)FROMBIT(bitno), 0, PAGE_SIZE);
#endif	
}

static void bm_clearbit (u32 bitno, int init_area)
{
	int index = (bitno/32);
	int offset = (bitno%32);
	u32 mask = 1 << offset;
	free_bitmap[index] &= ~mask;
#ifdef GANOID_DEBUG
	if (init_area)
		memset ((void*)FROMBIT(bitno), (int)0xDEADBEEF, PAGE_SIZE);
#endif	
}

static u32 bm_getbit (u32 bitno)
{
	int index = (bitno/32);
	int offset = (bitno%32);
	u32 mask = 1 << offset;
	return (free_bitmap[index] & mask);
}

static int find_first_free (u32 *bit, u32 from) 
{
	u32 start_bit  = from;
	u32 end_bit = GETBIT(bm_heap_end);
	int index = (start_bit/32);
	int offset = (start_bit%32);
	u32 mask = 1 << offset;
	bool free_bit_found = 0;

	if (start_bit > end_bit)
		return 1;

	//printk ("start_bit = %u, end_bit = %u\n", start_bit, end_bit);
	
	u32 i = kdebug_init_val;
	for (i = start_bit; i <= end_bit; index++){
		for (; mask && (i <= end_bit); mask = mask<<1, i++) {
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


static int find_free_bits_range (u32 total_bits, u32 *start_bit)
{
	u32 first_free_bit = kdebug_init_val;
	u32 l_start_bit = GETBIT(bm_heap_start);
	u32 end_bit = GETBIT(bm_heap_end);
	bool success = 0;


	//printk ("%s\n", __FUNCTION__);
	//printk ("l_start_bit: %u\ntotal_bits: %u\nend_bit: %u\n", l_start_bit, total_bits, end_bit);



	u32 i = kdebug_init_val;
	u32 b = kdebug_init_val;
	u32 remaining_bits = total_bits - 1;
	for (;(l_start_bit + remaining_bits) <= end_bit;){
		//printk ("Checking from bit %d ... ", l_start_bit);
		if (0 == find_first_free(&first_free_bit, l_start_bit)){
			// we already have one bit free. now
			// we need to check for remaining
			// bits in this range
			//printk ("first free = %d .... others ...", first_free_bit);
			for (b = first_free_bit + 1, i = 0; (i < remaining_bits) && 
				     (b <= end_bit); i++, b++){ 
				//printk (" ...%d.", b);
				if (bm_getbit (b))
					break;
			}
			if (i == remaining_bits){
				//printk ("..Success\n");
				success = 1;
				break;
			}
		}
		//printk ("Fail\n");
		l_start_bit = first_free_bit+i;
	}
	//printk ("done\n");
	if (success){
		*start_bit = first_free_bit;
		return 0;
	}
	else
		return 1;
}

static int find_free_bits_range_from (u32 total_bits, u32 start_bit)
{
	u32 first_free_bit;
	u32 l_start_bit = start_bit;
	u32 end_bit = GETBIT(bm_heap_end);
	bool success = 0;

	
	printk ("Heap size requested = 0x%x bytes, starting from address = 0x%x,"
		"upto address 0x%x\n", FROMBIT(total_bits), FROMBIT(start_bit), 
		FROMBIT(end_bit)+PAGE_SIZE-1);

	printk ("start_bit = %u, end_bit = %u, total_bits = %u\n", 
		start_bit, end_bit, total_bits);

	u32 i = kdebug_init_val;
	if ((l_start_bit + total_bits - 1) <= end_bit){
		if (0 == find_first_free(&first_free_bit, l_start_bit)){
			if (first_free_bit == l_start_bit){
				for (i = 1; i < total_bits; i++){
					if (bm_getbit (first_free_bit+i))
						break;
				}
				if (i == total_bits){
					success = 1;
				}
				
			}
		}
		
	}
	if (success){
		printk ("Success\n");
		return 0;
	}
	else{
		printk ("Fail\n");
		return 1;
	}
}



void* bm_malloc (u32 size)
{
	_ASSERT(bootmem_init_complete, EBootmemAllocatorNotInitialized,
		0, 0, 0);
	u32 r;
	//size = 17000;
 	u32 rsize = ROUND_TO_PAGE_SIZE(size);
	u32 bits = rsize/PAGE_SIZE;
	u32 start_bit = kdebug_init_val;

	// first check if we have space left in our account area
	bm_account *s = NULL;
	s = search_free_acnt_node ();
	if (!s) {
		//printk ("No free account node found. Cannot allocate memory !\n");
		return NULL;
	}
	//	else
	//printk ("Free account node found....\n");

	//printk ("bits = %d\n", bits);
	r = find_free_bits_range (bits, &start_bit);
	//printk ("r = %d\n", r);
	if (r)
		return NULL;

	u32 end_bit = start_bit + bits - 1;

	bm_setbit_range (start_bit, end_bit, 1);


	s->start_bit = start_bit;
	s->end_bit = end_bit;
	acnt_i++;
	//	printk ("Bootmem allocated space: from %u to %u\n", start_bit, end_bit);

	return (void*)FROMBIT(start_bit);;
}


void bm_free (void *ptr)
{
	_ASSERT(bootmem_init_complete, EBootmemAllocatorNotInitialized,
		0, 0, 0);
	u32 bit = GETBIT(ptr);

	if (ptr) {
		bm_account *s = NULL;
		s = search_bit_in_acnt (bit);
		_ASSERT ((s), EMultibootFreeAddrInvalid, (u32)ptr,
			 bit, 0);

		bm_clearbit_range (s->start_bit, s->end_bit, 1);
		//		printk ("Bootmem deallocated space: from %u to %u\n", 
		//s->start_bit, s->end_bit);
		
		s->start_bit = 0;
		s->end_bit = 0;
		acnt_i--;
	}
}



// find if the current bit is in the account area. 
bm_account* search_bit_in_acnt (u32 bit)
{
	int r = 1;
	bm_account *s = start_addr_acnt;
	//DUMP_WORD (s);

	//printk ("trying to find bit %u ..", bit);
	for (;(s <= end_addr_acnt);s++){
		// printk ("kaus: %u %u\n", s->start_bit, s->end_bit); 
		if (bit <= s->end_bit)
			if (bit >= s->start_bit){
				//printk ("found .. ");
				r = 0;
				break;
			}
	}

	if (!r) {
		//printk ("s->start_bit = %u s->end_bit = %u\n", s->start_bit, s->end_bit);
		return s;
	}
	else{
		//printk ("not found\n");
		return NULL;
	}
}


// search free account information node ..
bm_account* search_free_acnt_node ()
{
	// search for '0' because whichever node will have
	// bit 0 will be considered empty according to our
	// implicit understanding.
	return search_bit_in_acnt (0);
}
