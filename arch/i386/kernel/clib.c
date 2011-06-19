#include <stdarg.h>
#include <vga.h>
#include <ganoid/types.h>
#include <clib.h>
#include <string.h>
#include <kbd-handler.h>

#define isdigit(c) ((c >= '0') && (c <= '9'))

void *memset (void *s, int c, size_t n)
{
  size_t i;
  void *rs;
  unsigned char *s1 = (unsigned char*)s;
  rs = s;
  for (i = 0; i < n; i++, s1++){
	  *s1 = (unsigned char)c;
  }
  return rs;
}


int atoi (const char *nptr)
{
  int i;
  int val;
  int t = 1;
  int flag = 0;

  // first check if there is a sign at the start
  switch(*nptr){
  case '+':
    nptr++;
    break;
  case '-':
    flag = 1;
    nptr++;
    break;
  }

  // first go upto the units digit.
  for (i=0;*nptr; nptr++,i++){
    if (isdigit(*nptr)){
      continue;
    }
    else
      break;
  }

  // now get the decimal no.
  nptr = nptr-1; // to point to the last digit
  // now we will parse the digit in reverse from left to right.
  // value of 'i' is set in above loop. this value signifies the number of digits
  // in the given number.
  for (t = 1, val=0; i--; nptr--, t *= 10){
    if (!isdigit (*nptr)){
      vga_puts ("something is wrong \n");
      while(1);
    }
    val +=( (*nptr&0x0F) * t);
  }

  if (flag){ // the value needs to be converted to negative, by two's complement.
    val = ~val;
    val++;
  }

  return val;
}

void *memcpy(void *dest, const void *src, size_t n)
{
  void *retdest = dest;
  const char *csrc = src;
  char *cdest = dest;

  for (;n--; cdest++, csrc++)
    *cdest = *csrc;

   return retdest;
}


char *strcpy(char *dest, const char *src)
{
  char *retstr = dest;
  for (; *src; dest++, src++)
    *dest = *src;

  *dest = 0;

  return retstr;
}


char *strncpy (char *dest, const char *src, size_t n)
{
	size_t i;
	char *d = dest;
	for(i = n; (*dest = *src) && i; i--, src++, dest++){}
	return d;
}



int strlen (const char *s)
{
	int i=0;
	while (*s++) i++;
	return i;
}



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
	//printf ("Swap: %d with %d\n", *element1, *element2);
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
