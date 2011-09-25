/*
 * mm/free-pages.c
 * 
 * Physical page alloc/free database
 * 
 * Copyright (C) 2011 Kaustubh Ashtekar
 */

#include <ganoid/types.h>
#include <kernel.h>
#include <sysinfo.h>
#include <bootmem.h>
#include <kdebug.h>
#include <mm.h>
#include <string.h>



/*
 * There are NO_OF_PAGE_FRAMES pages available. Not all of them      
 * are usable as RAM as some of the physical address space is mapped
 * to other hardware devices.
 *
 * We keep two arrarys. One of storing the available address space
 * and the other for storing the allocated pages.
 */

static u32 ram_bm[NO_OF_PAGE_FRAMES/32];
static u32 alloc_bm[NO_OF_PAGE_FRAMES/32];


extern char __start_, __end_;


#define GETBIT(a) (u32)(a)/PAGE_SIZE
#define FROMBIT(b) (u32)(b*PAGE_SIZE)


static void setbit(u32 *bm, u32 bitno)
{
    u32 index = (bitno/32);
    u32 offset = (bitno%32);
    u32 mask = (u32)(1 << offset);
    //printk ("setting bit %d\n .. alloc_bm[index] = 0x%8x\n", bitno, alloc_bm[index]);
    bm[index] |= mask;
    /*printk ("Setbit %u in alloc_bm[%d] = 0x%8x Start_Addr = 0x%8x\n",bitno, 
      index, alloc_bm[index], FROMBIT(bitno));*/
}

static void clearbit(u32 *bm, u32 bitno)
{
    u32 index = (bitno/32);
    u32 offset = (bitno%32);
    u32 mask = (u32)(1 << offset);
    bm[index] &= ~mask;
}

static void setbit_range(u32 *bm, u32 start_bit, u32 end_bit)
{
    //printk ("setbit range: %u to %u\n", start_bit, end_bit);
    for (u32 i = start_bit; i <= end_bit; i++) 
	setbit(bm, i);
}

static void clearbit_range(u32 *bm, u32 start_bit, u32 end_bit)
{
    for (u32 i = start_bit; i <= end_bit; i++) 
	clearbit(bm, i);
}

static u32 getbit(const u32 *bm, u32 bitno)
{
    u32 index = (bitno/32);
    u32 offset = (bitno%32);
    u32 mask = (u32)(1 << offset);
    return (bm[index] & mask);
}

static int find_first_free (u32 *bit, u32 from_bit, u32 max_bit)
{
    u32 index = (from_bit/32);
    u32 offset = (from_bit%32);
    u32 mask = (u32)(1 << offset);
    bool free_bit_found = 0;

    u32 i;
    for (i = from_bit; i <= max_bit; index++){
	if(!ram_bm[index])
	    continue;
	if(!alloc_bm[index]) {
	}
	    
	for (; mask && (i <= max_bit); mask = mask<<1, i++) {
	    if (!(alloc_bm[index] & mask)){
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
		if (getbit (b))
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
		    if (getbit (first_free_bit+i))
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

	


/** \brief Initialize the boot memory allocator
 *
 * This takes RAM information from the multiboot header and then sets up some
 * data structures to manage this RAM
 */
void init_free_page_data(void)
{
    u32 rammap_node_count;
    int r = 0;

    u32 *ganoid_start = &__start_;
    u32 *ganoid_end = &__end_;
    
    memset(ram_bm, 0x00, sizeof(ram_bm));
    memset(alloc_bm, 0x00, sizeof(ram_bm));

    const ram_map *s = get_rammap_ptr(&rammap_node_count);
    printk ("Following information about RAM address map received from multiboot:\n");
    for (u32 i  = 0; i < rammap_node_count; i++) {
	printk ("%d: Start: 0x%8x End: 0x%8x\n", 
		i, s[i].start_addr, s[i].end_addr);
	_ASSERT_DEBUG(!((u32)start_addr & 0xFFF), 
		      EMultibootRAMAddrBad, 
		      (u32)start_addr, (u32)end_addr, i);
	setbit_range(ram_bm, GETBIT(s[i].start_addr, 
				    GETBIT(s[i].end_addr)));
	
	if(s[i].start_addr < ganoid_start &&
	   ganoid_end < s[i].end_addr) {
	    setbit_range(alloc_bm, GETBIT(ganoid_start),
			 GETBIT(ganoid_end));
	    printk ("Area occupied by Ganoid:\n");
	    printk ("Start: 0x%8x\n", ganoid_start);
	    printk ("End  : 0x%8x\n", ganoid_end);
	}
    }
}

#if 0
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

    setbit_range (start_bit, end_bit, 1);


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

	clearbit_range (s->start_bit, s->end_bit, 1);
	//		printk ("Bootmem deallocated space: from %u to %u\n", 
	//s->start_bit, s->end_bit);
		
	s->start_bit = 0;
	s->end_bit = 0;
	acnt_i--;
    }
}
#endif // 0
