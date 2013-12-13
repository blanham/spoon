#ifndef _KERNEL_DIV_H
#define _KERNEL_DIV_H


#include <inttypes.h>

static inline uint32_t udiv( uint64_t dividend, uint32_t divisor, uint32_t *rem )
{
	uint32_t quotient;
	uint32_t remainder;
		
	
	asm volatile 
			(  "mov %2, %%edx\n"
			   "mov %3, %%eax\n"
			   "mov %4, %%ecx\n"
			   "divl %%ecx\n"
			   "mov %%eax, %0\n"
			   "mov %%edx, %1\n"

			  : "=g" (quotient), "=g" (remainder)
			  : "g" ( ((uint32_t)((dividend) >> 32) & 0xFFFFFFFF) ),
			    "g" (  (uint32_t) (dividend         & 0xFFFFFFFF)  ),
				"g" ( divisor )
			  
			  : "eax", "edx", "ebx" 
			  );

	*rem = remainder;
	return quotient;
}


#endif

