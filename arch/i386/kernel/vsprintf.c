#include <stdarg.h>
#include <kernel.h>
#include <ktypes.h>
#include <string.h>

#include <vga.h>

#define ZEROPAD	1		/* pad with zero */
#define SIGN	2		/* unsigned/signed long */
#define PLUS	4		/* show plus */
#define SPACE	8		/* space if plus */
#define LEFT	16		/* left justified */
#define SPECIAL	32		/* 0x */
#define SMALL	64		/* use 'abcdef' instead of 'ABCDEF' */


#define isdigit(c) ((c >= '0') && (c <= '9'))

int debug_printf = 0;


// ignoring base for now.
char * itoa (int value, char *str, int base, int unsignedvalue, int fieldwidth)
{
	long i = 1,j, div;
	long no_of_digits = 1;
    int d = ' ';
    char carray[]="0123456789ABCDEF";
    char *retstr = 0;
    retstr = str;
    unsigned  uvalue;
    unsigned abs_value;
    int fillbytes = 0;

    abs_value = (unsigned)value;

    if( 
	   (value & 0x80000000) &&
       (base != 16) &&
       (base != 2) &&
       (!unsignedvalue)
	) {
	// this no. is a negative number
	*str = '-';
	str++;

	// now do a two complement
	abs_value = ~((unsigned)abs_value);
	abs_value +=1;
    }

    uvalue = abs_value;

    // calculate the no. of digits
    for (i = 1; uvalue /= base; i++){}

    no_of_digits = i;

    if (fieldwidth > 0){
	if ((fieldwidth - no_of_digits) < 0) // digits will not fit in given fieldwidth
	    fillbytes = 0;
	else
	    fillbytes = fieldwidth - no_of_digits;

	for (;fillbytes--; str++)
	    *str = '0';
    }

    // calculate the divisor
    i--;
    for (j = i, div = 1;j;j--, div *= base){}

    for (;div; div /= base, str++){
		d = (abs_value / div); // get the digit
	*str = carray[d];
		abs_value = abs_value - (d*div);
    }

    if (fieldwidth < 0){
	if (((-fieldwidth) - no_of_digits) < 0) // digits will not fit in given fieldwidth
	    fillbytes = 0;
	else
	    fillbytes = (-fieldwidth) - no_of_digits;

	for (;fillbytes--; str++)
	    *str = '.';
    }

    *str = 0; // append a NULL

    return retstr;
}

int vsprintf (char *buf, const char *fmt, va_list vl)
{
    // char ch;
    int flags = 0;
    int fieldwidth = -1;
    int precision = 0;
    int length_modifier = -1;
    int conv_specifier = 0;

    char *mbuf;
    mbuf = buf;

    while (1) {
	if (0 != uptonext (&buf, &fmt)) // traverse upto next '%'
	    break;
	fmt++;
	if (0 == *fmt){
	    *buf = 0; // terminate the string
	    break;
	}
	getflags (&flags, &fmt);
	if (isdigit(*fmt)){ // if true fieldwidth is present
	    fieldwidth = get_value (&fmt);
	}
	else if('*' == *fmt){ // fieldwidth is provided as argument
	    fieldwidth = va_arg (vl, int);
	    fmt++;
	}

	if (flags & LEFT)
	    fieldwidth = -fieldwidth;

	// now comes the precision field
	if('.' == *fmt){
	    // precision field needs to be parsed.
	    fmt++;
	    precision = get_value (&fmt);
	}

	length_modifier = get_lengthmodifier (&fmt);
	conv_specifier = get_conv_specifier (fmt);
	fmt++;
	realize_string (&buf, flags, fieldwidth, precision, length_modifier, conv_specifier, &vl);
	fieldwidth = 0;
	precision = 0; 
	length_modifier = 0; 
	conv_specifier = 0;
	//vga_puts (mbuf);
    }
    return strlen (buf); 
}


void realize_string (char **buf, int flags, int fieldwidth, 
		     int precision, int length_modifier, int conv_specifier, va_list *vl)
{
    char *mbuf;
    char *ch;
    char ch1;
    int val = 0;
#define STR_SIZE 100
    char str[STR_SIZE];
    memset (str, 0, STR_SIZE);
#undef STR_SIZE
    int i;


    mbuf = *buf;
    switch (conv_specifier){
    case 's': 
	ch = va_arg (*vl, char*);
	while (*ch)
	    *mbuf++ = *ch++; // copy entire string.
	break;
    case 'd':
	val = va_arg (*vl, int);
	itoa (val, str, 10, 0, fieldwidth);     
	break;
    case 'x':
	val = va_arg (*vl, int);
	itoa (val, str, 16, 1, fieldwidth);     
	break;
    case 'u':
	val = va_arg (*vl, int);
	itoa (val, str, 10, 1, fieldwidth);     
	break;
    case 'c':
		ch1 = va_arg (*vl, int);
	*mbuf++ = ch1;
	break;
    default:
	*mbuf = 0;
	vga_puts(mbuf);
		vga_putc (conv_specifier);
	vga_putc(':');
	vga_puts("conversion specifier not supported yet\n");
	while (1);
    }
    for(i = 0; str[i]; i++, mbuf++)
	*mbuf = str[i];
  
    *mbuf = 0;
    *buf = mbuf;
}

int get_conv_specifier (const char *fmt)
{
    int retval = -1;
    switch (*fmt) {
    case 'd':
    case 'i':
	retval= 'd';
	break;
    case 'o':
    case 'u':
    case 'x':
    case 'X':
    case 'c':
    case 's':
	retval = *fmt;
	break;
    }

    return retval;
}

int get_lengthmodifier (const char **fmt)
{
    const char *mfmt;
    mfmt = *fmt;
    int retval = -1;
    switch (*mfmt){
    case 'l':
	retval = *mfmt;
	mfmt++;  
	break;
    }

    *fmt = mfmt;
    return retval;
}


/** \brief Return the integer value
 * Parse the given string from the given pointer until all
 * contiguous digits have ended. Return the number.
 */
// /*inline*/
int get_value (const char **fmt)
{
    int i;
    int val;
    const char *mfmt;
    int t = 1;
    mfmt = *fmt;

    //  vga_puts (mfmt);

    // first go upto the units digit.
	for (i=0;*mfmt != 0; *mfmt++,i++){
		if (isdigit(*mfmt))
			continue;
		else
			break;
	}

    *fmt = mfmt; 

    // now get the decimal no.
    mfmt = mfmt-1; // to point to the last digit
    // now we will parse the digit in reverse from left to right.
    // value of 'i' is set in above loop. this value signifies the number of digits
    // in the given number.
    for (t = 1,val=0; i--; *mfmt--, t *= 10){
	val +=( (*mfmt&0x0F) * t);
    }

    return val;
}


/** \brief Extract the flags.
 *
 */
/*inline*/ int getflags (int *flags, const char **fmt)
{
    const char *mfmt;
    mfmt = *fmt;

    for (;*mfmt;mfmt++){
	switch (*mfmt){
	case '-': *flags |= LEFT;break;
	case '+': *flags |= PLUS;break;
	case ' ': *flags |= SPACE;break;
	case '#': *flags |= SPECIAL;break;
	case '0': *flags |= ZEROPAD;break;
	default:
	    goto getflags_done;
	}
    }

 getflags_done:
    *fmt = mfmt;
    return 0;
}

/** \brief Take the string upto next '%'
 *
 */
/*inline*/ int uptonext (char **buf, const char **fmt)
{
    int retval = -1;
    const char *mfmt = *fmt;
    char *mbuf = *buf;
    while (*mfmt){
	if ('%' == *mfmt){
	    retval = 0;
	    break;
	}
	*mbuf++ = *mfmt++;
    }

    // append a NULL
    if (!*mfmt)
	*mbuf = 0;

    *fmt = mfmt;
    *buf = mbuf;

    return retval;
}




int sprintf (char *buf, const char *fmt, ...)
{
    va_list vl;
    int retval = 0;
  
    va_start (vl, fmt);
    retval = vsprintf (buf, fmt, vl);
    va_end (vl);

    return retval;
}



int printf (const char *fmt, ...)
{
    va_list vl;
    int retval = 0;

#define PRINTF_BUFFER_SIZE 1000
    char pbuf[PRINTF_BUFFER_SIZE];

    va_start (vl, fmt);
    retval = vsprintf (pbuf, fmt, vl);
    va_end (vl);

    vga_puts (pbuf);

    return retval;
}

