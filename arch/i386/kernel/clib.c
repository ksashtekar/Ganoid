#include <stdarg.h>
#include <vga.h>
#include <ktype.h>
#include <clib.h>
#include <string.h>

#define isdigit(c) ((c >= '0') && (c <= '9'))

void *memset (void *s, int c, size_t n)
{
  int i;
  void *rs;
  rs = s;
  for (i = 0; i < n; i++, s++){
    *(char*)s = c;
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



size_t strlen (const char *s)
{
	size_t i=0;
	while (*s++) i++;
	return i;
}





