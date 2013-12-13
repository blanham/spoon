#ifndef _KERNEL_STATS_H
#define _KERNEL_STATS_H


#ifndef _HAS_RDTSC_H
#define _HAS_RDTSC_H
static inline uint64_t rdtsc()
{
   uint64_t x;
   asm volatile (".byte 0x0f,0x31" : "=A" (x));
   return x;
}
#endif



/** structure for high-resolution timing */
struct s_time
{
	uint64_t ops;
	uint64_t last_rdtsc;
	uint64_t usage;
};


void stats_time( unsigned int cpu_id, struct s_time *st );

static inline void stats_time_start( struct s_time *st )
{
	st->last_rdtsc = rdtsc();
}




#endif

