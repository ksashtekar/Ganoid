#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdarg.h>
#include <ganoid/types.h>
#include <processor.h>

enum desc_privilege_level {

	DESC_SVC = 0,
	DESC_UNUSED_1,
	DESC_UNUSED_2,
	DESC_USER
};

int sprintf(char *buf, const char *fmt, ...);
int vsprintf(char *buf, const char *fmt, va_list vl);
int uptonext(char **buf, const char **fmt);
int getflags(int *flags, const char **fmt);
int get_value(const char **fmt);
int get_lengthmodifier(const char **fmt);
int get_conv_specifier(const char *fmt);
void realize_string(char **buf, int flags, int fieldwidth,
		    int precision, int length_modifier, int conv_specifier,
		    va_list *vl);
char *itoa(int value, char *str, unsigned base, int unsignedvalue,
	   int fieldwidth);
int printk(const char *fmt, ...);

#define ENTER_CRITICAL_SECTION  unsigned int ___eflags_ = disable_interrupts()
#define EXIT_CRITICAL_SECTION   restore_interrupts(___eflags_)

#define HZ 1000

#define KERNEL_PANIC while (1){}

#endif // KERNEL_H_
