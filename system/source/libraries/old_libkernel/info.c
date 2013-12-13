#include <types.h>
#include <kernel.h>


/**  \defgroup INFO  Information and Accounting  
 *
 */

/** @{ */


int32 smk_info_system( struct system_information *si )
{
	return _sysenter( (SYS_INFO|SYS_ONE), (uint32)si, 0,0,0,0 );
}

int32 smk_info_process( int32 pid, struct process_information *pi )
{
	return _sysenter( (SYS_INFO|SYS_TWO), (uint32)pid, (uint32)pi, 0,0,0 );
}

int32 smk_info_thread( int32 pid, int32 tid, 
		   struct thread_information *ti )
{
	return _sysenter( (SYS_INFO|SYS_THREE), (uint32)pid, 
			 (uint32)tid, (uint32)ti, 0,0 );
}

int32 smk_spit()
{
	return _sysenter( (SYS_INFO|SYS_FIVE), 0, 0, 0, 0, 0 );
}



int32 smk_info_mem_free()
{
	struct system_information si;
	smk_info_system( &si );
	return si.memory_free;
}

int32 smk_info_mem_total()
{
	struct system_information si;
	smk_info_system( &si );
	return si.memory_total;
}


/** @} */

