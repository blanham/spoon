#ifndef _KERNEL_i686_THREAD_DATA_H
#define _KERNEL_i686_THREAD_DATA_H

#include <types.h>

struct thread_data
{
   uint32 stack_kernel;
   uint32 stack_user;
   uint32 stack_pages;
   uint32 stack;

   int32 math_state;
   uint8 *math;
  // uint8 math[512] __attribute__ ((aligned (1024)));   // fpu,mmx,xmm,sse,sse2
};


#endif

