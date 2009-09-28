#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdarg.h>

enum desc_privilege_level
	{
		DESC_SVC = 0,
		DESC_UNUSED_1,
		DESC_UNUSED_2,
		DESC_USER
	};


void delay (unsigned long n);

int sprintf (char *buf, const char *fmt, ...);
int vsprintf (char *buf, const char *fmt, va_list vl);
int uptonext (char **buf, const char **fmt);
int getflags (int *flags, const char **fmt);
int get_value (const char **fmt);
int get_lengthmodifier (const char **fmt);
int get_conv_specifier (const char *fmt);
void realize_string (char **buf, int flags, int fieldwidth, 
		     int precision, int length_modifier, int conv_specifier, va_list *vl);
char * itoa (int value, char *str, int base, int unsignedvalue, int fieldwidth);
int printf (const char *fmt, ...);

#endif // KERNEL_H_
