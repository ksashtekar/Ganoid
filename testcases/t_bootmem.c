#include <ganoid/types.h>
#include <stdarg.h>
#include <kernel.h>
#include <vga.h>
#include <clib.h>
#include <string.h>
#include <tests.h>
#include <bootmem.h>
#include <mm.h>


int Test_bootmem (int param1, int param2, void *ptr)
{
	int test_result = 0;
	printf ("Starting tests of bootmem allocator ... \n");
	delay (0);
	delay (0);

	// -----------------------------------------------
	// start with a simple allocation
#define MAX_ALLOC 3000
	uint *p_a[MAX_ALLOC];
	uint *p1, *p2, *p;

	uint i_a;

	for (int i = 0; i < MAX_ALLOC; i++) {
		p_a[i] = bm_malloc (4097);
		if (p_a[i] ==  NULL){
			printf ("Allocation fail for %d\n", i);
			i_a = i;
			break;
		}
	}
	for (int i = 0; i <= i_a; i++){
		bm_free(p_a[i]);
	}
	printf ("Allocation and deallocation upto %d completed successfully", i_a);

 	delay (0);
 	delay (0);
 	delay (0);
 	delay (0);


	for (int i = 0; i < MAX_ALLOC; i++) {
		p_a[i] = bm_malloc (4097);
		if (p_a[i] ==  NULL){
			printf ("Allocation fail for %d\n", i);
			i_a = i;
			break;
		}
	}
	for (int i = 0; i <= i_a; i++){
		bm_free(p_a[i]);
	}
	printf ("Allocation and deallocation upto %d completed successfully", i_a);

	ILP;


	if (!p1){
		test_result = 1;
		printf ("Allocation fail\n");
		//return test_result;
	}
	else
		printf ("Allocation success\n");

	p2 = bm_malloc (1);
	if (!p2){
		test_result = 1;
		printf ("Allocation fail\n");
		//return test_result;
	}
	else
		printf ("Allocation success\n");

	
// deallocate what we have just allocated
	bm_free (p2);
	bm_free (p1);
	ILP;
	// -----------------------------------------------


	// -----------------------------------------------
	// now try allocating more than one page
	p = bm_malloc (PAGE_SIZE+1);
	if (!p){
		test_result = 1;
		return test_result;
	}

	// deallocate what we have just allocated
	bm_free (p);
	// -----------------------------------------------


	while (1);
	// -----------------------------------------------
	// allocate till bootmem heap is over. then
	// deallocate last allocation and try allocating it
	// again. 
	while ((p = bm_malloc(PAGE_SIZE))) {}
	bm_free(p);
	p = bm_malloc(PAGE_SIZE);
	if (!p){
		test_result = 1;
		return test_result;
	}

	printf ("All test pass\n");
	while (1){}
}
