#include <cpu.h>
#include <time.h>
#include <stats.h>
#include <div.h>


void stats_time( unsigned int cpu_id, struct s_time *st )
{
	uint64_t temp = rdtsc();


	if ( st->last_rdtsc == 0 )
	{
		st->last_rdtsc = temp;
		return;
	}
	
	
	st->ops += temp - st->last_rdtsc;
	st->last_rdtsc = temp;
	
	// update time usage --------------------

		if ( st->ops >= cpu[ cpu_id].bogomips )
		{
			uint32_t rem;
			st->usage += udiv( st->ops, cpu[ cpu_id].bogomips, &rem );
			st->ops   = rem;
		}
}



