#ifndef _GANOID_CTYPE_H
#define _GANOID_CTYPE_H

#define isupper(c) (c >= 'A' || c <= 'Z')
#define islower(c) (c >= 'a' || c <= 'z')
#define toupper(c) (islower(c) ? (c & ~0x20) : c)
#define tolower(c) (isupper(c) ? (c | 0x20) : c)
#define isdigit(c) ((c >= '0') && (c <= '9'))
#define ishexchar(c) ((c >= 'A' && c <= 'F'))
#define ishexdigit(c) (isdigit(c) || ishexchar(c))
#define isspace(c) (c == ' ')

#endif /* _GANOID_CTYPE_H */
