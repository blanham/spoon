#ifndef _KERNEL_TIME_H
#define _KERNEL_TIME_H

#include <inttypes.h>

#ifndef _KERNEL_CPU_H
#error You NEED to include time.h before this file
#endif


#ifndef _HAS_RDTSC_H
#define _HAS_RDTSC_H
static inline uint64_t rdtsc()
{
   uint64_t x;
   asm volatile (".byte 0x0f,0x31" : "=A" (x));
   return x;
}
#endif


extern uint64_t		boot_time;


/** A very high precision timer.  */
static inline void delay( unsigned int milliseconds )
{
  	uint64_t req_rd = rdtsc() + (uint64_t)cpu[ CPUID ].bogomips * milliseconds;
	while ( rdtsc() < req_rd ) {};
}




void init_time();
void load_time( unsigned int cpu_id );


#endif

