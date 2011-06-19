#ifndef KTYPE_H_
#define KTYPE_H_

#define GANOID_DEBUG 
#define ILP while(1);

#define OS_NAME "Ganoid"

typedef unsigned short u16;
typedef unsigned char u8;
typedef char          i8;
typedef unsigned int u32;
typedef unsigned int size_t;


typedef unsigned char uchar;
typedef unsigned int u32;
typedef int bool;
typedef int pid_t;

#define NULL ((void*)0)

#define MAX_UINT ~(u32)0x00

#define true  1
#define TRUE  1
#define false 0
#define FALSE 0

extern const u32 kdebug_init_val;

#endif // KTYPE_H_
