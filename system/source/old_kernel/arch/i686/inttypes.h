#ifndef _KERNEL_ARCH_INTTYPES_H
#define _KERNEL_ARCH_INTTYPES_H

/*

  This defines some types depending on the amount of bits
  each type has. 

*/


#ifndef NULL
#define NULL (void*)0
#endif


typedef unsigned long long uint64;     // 8 bytes - 4 words - eax + eax
typedef unsigned int uint32;           // 4 bytes - 2 words - eax
typedef unsigned short uint16;         // 2 bytes - 1 word  -  ax
typedef unsigned int uint;             // 2 bytes - 1 word  -  ax
typedef unsigned char uint8;           // 1 byte  - 1 byte  - ah/al

typedef long long int64;     // 8 bytes - 4 words - eax + eax
typedef int int32;           // 4 bytes - 2 words - eax
typedef short int16;         // 2 bytes - 1 word  -  ax
typedef char int8;           // 1 byte  - 1 byte  - ah/al


typedef int32  intptr;		 
typedef uint32 uintptr;		

#define TO_INTPTR(ptr)  ((intptr)(ptr))
#define TO_UINTPTR(ptr)  ((uintptr)(ptr))
#define TO_PTR(intptr)  ( (void*)(intptr & (0xFFFFFFFFul)) )


typedef uint32 time_t;




typedef struct 
{
	uint32 lock;
	struct thread* thread;
} spinlock;

#define INIT_SPINLOCK  { 0, NULL }
#define INLINE_INIT_SPINLOCK(spinner)	\
				spinner.lock = 0;		\
				spinner.thread = NULL	
				
typedef struct {  void* pde; } vmmap;


#endif


