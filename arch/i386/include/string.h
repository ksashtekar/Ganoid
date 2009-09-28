#ifndef STRING_H_
#define STRING_H_

#include <ktype.h>

void *memset (void *s, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
char *strcpy(char *dest, const char *src);
size_t strlen (const char *s);


#endif //STRING_H_
