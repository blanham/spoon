#include <kernel.h>

		
/**  \defgroup SHMEM  Shared Memory  


<P>
Shared memory is a section of physical memory which is mapped into more than one processes
address space.  The regions may not have the same virtual address in any process but
any write by one process will immediately appear in the other processes memory map (Because
it's the same piece of memory.).



<H4>Example: Using both Pulses and Shared Memory to communicate a large message</H4>

<P>
A process can share a section of memory with B process. A then sends a pulse to B with the
remote shared memory id. B receives the id, accepts the memory and then reads the huge 
section that A process shared with it.  Once B is done with the data, it releases the shared memory.


 
  

  
 */
		  

/** @{ */


int smk_create_shmem( const char name[ OS_NAME_LENGTH ], 
						int pages, 
						unsigned int flags )
{
	return _sysenter( (SYS_SHMEM|SYS_ONE),
					  (uint32)name, (uint32)pages, flags, 0,0 );
}


int smk_grant_shmem( int id, int pid, unsigned int flags )
{
	return _sysenter( (SYS_SHMEM|SYS_THREE),
					  0,
					  (uint32)id,
					  (uint32)pid,
					  (uint32)flags,
					  0 );
}


int smk_revoke_shmem( int id, int pid )
{
	return _sysenter( (SYS_SHMEM|SYS_THREE),
					  1,
					  (uint32)id,
					  (uint32)pid,
					  0,
					  0 );
}


int smk_request_shmem( int id, void **location, int *pages, unsigned int *flags )
{
	return _sysenter( (SYS_SHMEM|SYS_FOUR),
						0,
						(uint32)id,
						(uint32)location,
						(uint32)pages,
						(uint32)flags );
}

int smk_release_shmem( int id )
{
	return _sysenter( (SYS_SHMEM|SYS_FOUR), 1, (uint32)id, 0, 0, 0 );
}

int smk_delete_shmem( int id )
{
	return _sysenter( (SYS_SHMEM|SYS_TWO), (uint32)id, 0, 0,0,0 );
}

int smk_find_shmem( const char name[ OS_NAME_LENGTH ], int *pid )
{
	return _sysenter( (SYS_SHMEM|SYS_FIVE), (uint32)name, (uint32)pid,0,0,0 );
}


int smk_get_shmem_info( int id, char name[ OS_NAME_LENGTH ],
							int *pid, int *pages, unsigned int *flags )
{
	return _sysenter( (SYS_SHMEM|SYS_SIX), (uint32)id,
						(uint32)name, (uint32)pid, (uint32)pages,
						(uint32)flags );
}



/** @} */



