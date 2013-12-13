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

void *smk_mem_alloc( unsigned int pages )
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
      _sysenter( (SYS_MEMORY|SYS_ONE), 1, (unsigned int)pointer, 0, 0, 0 ); 
}


/** Returns the size of the allocated memory at the pointer address
 * given. This pointer must be exactly the same address that was
 * returned by smk_mem_alloc.
 */
int smk_mem_size( void *pointer )
{
	return _sysenter( (SYS_MEMORY|SYS_THREE), (unsigned int)pointer,0,0,0,0 );
}


/** Returns the physical address of the memory addressed by pointer. 
 * This would only be useful to you if you're writing a device driver.
 * */
void *smk_mem_location( void *pointer )
{
	
	return (void*)(_sysenter( (SYS_MEMORY|SYS_FOUR), (unsigned int)pointer,0,0,0,0 ));
}

/** This requests the kernel to linearly map the physical address 
 * of pages page size into your application memory space. This is 
 * also useful if you're writing a device driver. 
 */
void *smk_mem_map( void *location, int pages )
{
	return (void*)(_sysenter( (SYS_MEMORY|SYS_TWO), 
								(unsigned int)location, pages, 0,0, 0 ));
}


/** @} */

