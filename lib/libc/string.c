#include <ganoid/string.h>

void *memset(void *s, int c, size_t n)
{
	char *p = s;
	for (size_t i = 0; i < n; i++, p++)
		*p = (char)c;
	return s;
}

void *memcpy(void *dest, const void *src, size_t n)
{
	char *d = dest;
	const char *s = src;
	for (size_t i = 0; i < n; i++, d++, s++)
		*d = *s;
	return dest;
}

void *memmove(void *dest, const void *src, size_t n)
{
	const char *s = src;
	char *d = dest;
	if (d > s && d < (s + n)) {
		/* overlap case */
		s = s + n - 1;
		d = d + n - 1;
		while (1) {
			*d = *s;
			if (s == src)
				break;
			d--;
			s--;
		}
	}

	return dest;
}

char *strcpy(char *dest, const char *src)
{
	char *s_dest = dest;
	while (1) {
		*dest = *src;
		if (!*src)
			break;
		src++;
		dest++;
	}

	return s_dest;
}

char *strncpy(char *dest, const char *src, size_t n)
{
	char *s_dest = dest;
	while (n--) {
		*dest = *src;
		if (!*src)
			break;
		src++;
		dest++;
	}

	return s_dest;
}

size_t strlen(const char *s)
{
	size_t i = 0;
	for (; *s; s++, i++)
		;
	return i;
}
