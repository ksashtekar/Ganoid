#include <ganoid/stdlib.h>
#include <ganoid/ctype.h>
#include <vga.h>

int atoi(const char *nptr)
{
	int i;
	int val;
	int t = 1;
	int flag = 0;

	/* first check if there is a sign at the start */
	switch (*nptr) {
	case '+':
		nptr++;
		break;
	case '-':
		flag = 1;
		nptr++;
		break;
	}

	/* first go upto the units digit. */
	for (i = 0; *nptr; nptr++, i++) {
		if (isdigit(*nptr)) {
			continue;
		} else
			break;
	}

	/* now get the decimal no. */
	nptr = nptr - 1;	/* to point to the last digit */
	/* now we will parse the digit in reverse from left to right.
	   value of 'i' is set in above loop. this value signifies the
	   number of digits in the given number. */
	for (t = 1, val = 0; i--; nptr--, t *= 10) {
		if (!isdigit(*nptr)) {
			vga_puts("something is wrong.\n");
			while (1)
				;
		}
		val += ((*nptr & 0x0F) * t);
	}

	/* the value needs to be converted to negative, by two's complement. */
	if (flag) {
		val = ~val;
		val++;
	}

	return val;
}
