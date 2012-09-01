#include <stdarg.h>
#include <vga.h>
#include <ganoid/types.h>
#include <clib.h>
#include <ganoid/string.h>
#include <kbd-handler.h>


/* This is an implementation of the bubble sort algorithm. It will take an
   array of elements and return a sorted array. 
*/
void bubble_sort (int *input_array, int *output_array, int elements) 
{
	int last_pair = elements - 1;
	memcpy (output_array, input_array, ((unsigned int)elements*sizeof(int)));
	
	for (int i = 0; i < last_pair; i++) {
		for (int j = 0; j < last_pair; j++) {
			if (output_array[j] > output_array[j+1])
				swap_int (&output_array[j], &output_array[j+1]);
		} 
	}
	
}

void swap_int (int *element1, int *element2)
{
	//printk ("Swap: %d with %d\n", *element1, *element2);
	register int t = *element1;
	*element1 = *element2;
	*element2 = t;
}



char getc ()
{
	int ch;
	bool printable;
	int r = -1;
	while (1){
		r = read_key (&ch, &printable);
		if (0 == r) 
			if (printable == true) 
			    return (char)ch;

	}
}
