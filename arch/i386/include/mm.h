#ifndef _MM_H_
#define _MM_H_


#define PAGE_SIZE 4096

#define ROUND_TO_PAGE_SIZE(s) ((PAGE_SIZE*(((u32)s-1)/PAGE_SIZE)) + PAGE_SIZE)

//#define ROUND_TO_PAGE_SIZE(s) 

#endif // _MM_H_
