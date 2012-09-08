#include <ganoid/stdbool.h>
#include <ganoid/stdlib.h>
#include <ganoid/ctype.h>
#include <vga.h>
#include <errno.h>

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

/* Return -1 if invalid string (for base detection). Else returns base */
static int detect_base(const char **npptr)
{
	int base = -1;

	const char *nptr = *npptr;
	if (!*nptr)
		return base;

	if (*nptr == '0') {
		/* Can be hex, octal or garbage */
		nptr++;
		if (*nptr == 'x') {
			nptr++;
			/* Can be hex or garbage */
			if (ishexdigit(*nptr))
				base = 16;
		} else if (isdigit(*nptr)) {
			/* Is octal */
			base = 8;
		}
	}

	*npptr = nptr;
	return base;

}

/* Remove the leading '0x' if present. Returns error if the number string
   does not contain hex digits (0 - F)
*/
static int hex_process(const char **npptr)
{
	int result = -1;
	const char *nptr = *npptr;

	if (!*nptr)
		return result;

	if (*nptr == '0') {
		nptr++;
		/* This can be 'x' or a digit or garbage */
		if (*nptr == 'x') {
			nptr++;
		}
	}

	if (ishexdigit(*nptr))
		result = 0;

	*npptr = nptr;
	return result;
}

static bool is_correct_digit(char d, int base)
{
	switch (base) {
		/* arrange according to probability */
	case 10:
		return isdigit(d);
		break;
	case 16:
		return ishexdigit(d);
		break;
	case 2:
		return d == '0' || d == '1';
		break;
	default:
		return (isdigit(d) && ((d - 0x30) < base));
		break;
	}

	return false;
}

/* fixme: set errno for the current process*/
unsigned long long int simple_strtoull(const char *nptr,
				       char **endptr, int base)
{
	if (!nptr)
		return 0;
	if (base > 36 || base < 0 || base == 1)
		return 0;

	while (isspace(*nptr)) {
		nptr++;
	}

	bool neg_sign = false;
	if (*nptr == '+')
		nptr++;
	else if (*nptr == '-') {
		neg_sign = true;
		nptr++;
	}

	if (base == 0) {
		base = detect_base(&nptr);
		if (base == -1)
			return 0;
	}

	if (base == 16) {
		if (hex_process(&nptr) != 0)
			return 0;
	}

	/* gobble up the leading zeroes */
	while (*nptr == '0') {
		nptr++;
	}

	int mul = 1;
	unsigned long long num = 0;
	while (is_correct_digit(*nptr, base)) {
		unsigned digit = *(const unsigned char *)nptr;
		if (base == 16) {
			if (ishexchar(digit))
				digit = digit - 'A' + 10;
			else
				digit -= 0x30;
		} else
			digit -= 0x30;

		num = (unsigned long long)(num * (unsigned)mul) + digit;
		mul = base;
		nptr++;
	}

	if (endptr)
		*endptr = nptr;

	if (neg_sign) {
		/* two's complement */
		num = ~num;
		num++;
	}

	return num;
}

unsigned long int simple_strtoul(const char *nptr, char **endptr, int base)
{
	return (unsigned long)simple_strtoull(nptr, endptr, base);
}
