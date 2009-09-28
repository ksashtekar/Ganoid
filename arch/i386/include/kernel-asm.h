#ifndef KERNEL_ASM_H_
#define KERNEL_ASM_H_

#ifdef HAVE_ASM_USCORE
#define EXT_C(sym) _ ## sym
#else
#define EXT_C(sym) sym
#endif


#endif
