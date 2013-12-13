#include <kernel.h>



/**  \defgroup MEM  Memory Operations  
 *
 *
 * 
 */

/** @{ */


/**  Allocates blocks of memory. Set LARGE_PAGES to 0 if you
 *  want 4 KB pages or set it to 1 if you want 4 MB pages.
 * 
 *   Returns NULL if failure or the address of your memory.
 * 
 */

void *smk_mem_alloc( uint32 pages )
{
      return (void*)_sysenter( (SYS_MEMORY|SYS_ONE), 0, pages, 0, 0, 0); 
}

/** Frees previously allocated memory.  The pointer must be
 *  precisely the same as was given to you or else it will
 *  fail.
 *
 */

void smk_mem_free( void *pointer )
{
      _sysenter( (SYS_MEMORY|SYS_ONE), 1, (uint32)pointer, 0, 0, 0 ); 
}


int32 smk_mem_size( void *pointer )
{
	return _sysenter( (SYS_MEMORY|SYS_THREE), (uint32)pointer,0,0,0,0 );
}


void *smk_mem_location( void *pointer )
{
	
	return (void*)(_sysenter( (SYS_MEMORY|SYS_FOUR), (uint32)pointer,0,0,0,0 ));
}

void *smk_mem_map( void *location, int pages )
{
	return (void*)(_sysenter( (SYS_MEMORY|SYS_TWO), 
								(uint32)location, pages, 0,0, 0 ));
}


/** @} */

