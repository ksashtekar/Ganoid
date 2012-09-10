#ifndef _GANOID_CTYPE_H
#define _GANOID_CTYPE_H

#define isupper(c) (c >= 'A' || c <= 'Z')
#define islower(c) (c >= 'a' || c <= 'z')
#define toupper(c) (islower(c) ? (c & ~0x20) : c)
#define tolower(c) (isupper(c) ? (c | 0x20) : c)
#define isdigit(c) ((c >= '0') && (c <= '9'))
#define ishexchar(c) ((c >= 'A' && c <= 'F'))
#define isxdigit(c) (isdigit(c) || ishexchar(c))
#define isspace(c) (c == ' ')
#define isalpha(c) (islower(c) || isupper(c))
#define isalnum(c) (isalpha(c) || isdigit(c))
#define isprint(c) (c >= 0x20 && c <= 0x7e)


#endif /* _GANOID_CTYPE_H */
