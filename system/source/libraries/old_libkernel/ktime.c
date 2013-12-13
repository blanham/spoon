#include <kernel.h>


/**  \defgroup TIME  Time  
 *
 */

/** @{ */

uint32 smk_cmos_time()
{
	return _sysenter( (SYS_TIME|SYS_ONE), 0,0,0,0,0 );
}

int32 smk_system_time( uint32 *seconds, uint32 *milliseconds)
{
	return _sysenter( (SYS_TIME|SYS_TWO), (uint32)seconds, (uint32)milliseconds, 0,0,0 );
}

void smk_delay( uint32 milliseconds )
{
	_sysenter( (SYS_TIME|SYS_THREE), milliseconds, 0, 0,0,0);
}


void smk_sleep( uint32 seconds )
{
	_sysenter( (SYS_TIME|SYS_FOUR), seconds, 0, 0,0,0);
}



int smk_wait_process( int32 pid, int32 *rc )
{
   return _sysenter( (SYS_TIME|SYS_FIVE), pid, -1, (uint32)rc, 0, 0 );
}

int smk_wait_thread( int32 tid, int32 *rc )
{
   return _sysenter( (SYS_TIME|SYS_FIVE), smk_getpid(), tid, (uint32)rc, 0, 0 );
}

/** @} */


