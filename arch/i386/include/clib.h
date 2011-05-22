#ifndef CLIB_H_
#define CLIB_H_

#define FOR_COUNT(c,s) for(int i = 0; i < c; i++) s
	
void bubble_sort (int *input_array, int *output_array, int elements);
void swap_int (int *element1, int *element2);
int atoi (const char *nptr);
char getc (void);

#endif // CLIB_H_
