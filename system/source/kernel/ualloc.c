#include <inttypes.h>
#include <process.h>
#include <paging.h>
#include <alloc.h>
#include <ualloc.h>

#include <threads.h> // sk_BASE + CEILING
#include <physmem.h>

unsigned int user_alloc( struct process *proc, unsigned int pages)
{
   struct allocated_memory *mem;
   void *location;
   uint32_t found;
   
   if ( pages == 0 ) return 0;

    location = memory_alloc( pages );
	if ( location == NULL ) return 0;

	// Allocate this here to make failure returns easier later on.
    mem = malloc( sizeof( struct allocated_memory ) );
	if ( mem == NULL )
	{
		memory_free( pages, location );
		return 0;
	}
   
	found = page_provide( proc->map,
						  (unsigned int) location,
						  sK_BASE,
						  sK_CEILING,
						  pages,
						  USER_PAGE | READ_WRITE );
	
	if ( found == 0 )
	{
		memory_free( pages, location );
		free( mem );
		return 0;
	}
	
    mem->pages = pages;
    mem->flags = 0;
    mem->shared = 0;
    mem->phys = (unsigned int)( location );
    mem->virt = found;
    mem->prev = NULL;
    mem->next = proc->alloc;
    mem->parent = proc;
    proc->alloc = mem;
    if ( mem->next != NULL ) mem->next->prev = mem;

   
   return found;
}


static unsigned int user_alloc_hard( struct process *proc, 
								uint32_t virtual, 
								unsigned int pages
								)
{
   struct allocated_memory *mem;
   void *location;
   uint32_t found;
   
   assert( pages != 0 );

    location = memory_alloc( pages );
	if ( location == NULL ) return 0;

	// Allocate this here to make failure returns easier later on.
    mem = malloc( sizeof( struct allocated_memory ) );
	if ( mem == NULL )
	{
		memory_free( pages, location );
		return 0;
	}
   

	// A problem occurs here where ensure will ensure virtual regions.
	// However, since this is ualloc, we're keeping track of the
	// regions. This means that ensure may not overlap in this function.
	found = page_ensure( proc->map, virtual, pages, USER_PAGE | READ_WRITE );
	
	if ( found < 0 )
	{
		memory_free( pages, location );
		free( mem );
		return 0;
	}

	assert( found == 0 );	// We need to be 100% sure that we're not sharing.
	
    mem->pages = pages;
    mem->flags = 0;
    mem->shared = 0;
    mem->phys = (unsigned int)( location );
    mem->virt = found;
    mem->prev = NULL;
    mem->next = proc->alloc;
    mem->parent = proc;
    proc->alloc = mem;
    if ( mem->next != NULL ) mem->next->prev = mem;

   
   return found;
}


unsigned int user_free( struct process *proc, uint32_t address )
{
	struct allocated_memory *mem;
	int release_mem;
	
	
	  mem = proc->alloc;
	  while ( mem != NULL )
	  {
  	    if ( mem->virt == address) break;
	    mem = mem->next;
	  }

	  if ( mem == NULL ) return 0;

// remove mem from the process list.
	  if ( proc->alloc == mem ) proc->alloc = mem->next;
	  if ( mem->next != NULL ) mem->next->prev = mem->prev;
	  if ( mem->prev != NULL ) mem->prev->next = mem->next;

// orphan it
	  mem->parent = NULL;
	  
	release_mem = 1;

// Is it shared??
	  // yes => we can't release physical
	  if ( mem->shared > 0 ) release_mem = 0;
// Is it safe to release?
	  if ( (mem->flags & UALLOC_NORELEASE) != 0 ) release_mem = 0;

// Unmap it


		 if ( release_mem == 1 ) memory_free( mem->pages, (void*)mem->phys );
	
		 page_release( proc->map, mem->virt, mem->pages );	
  		 

	 if ( release_mem == 1 ) free( mem );

	  // -----------
	return 0;
}


/*
 *
 * user_size
 * 
 * Returns the size of an allocated region as identified
 * by pointer
 *
 */

int32_t user_size( struct process *proc, uint32_t pointer )
{
	struct allocated_memory *mem;
	
	  mem = proc->alloc;
	  while ( mem != NULL )
	  {
  	    if ( mem->virt == pointer) break;
	    mem = mem->next;
	  }

	  if ( mem == NULL ) return -1;

	  return mem->pages * PAGE_SIZE;
}

/*
 *
 * user_location
 * 
 * Returns the physical location of an allocated region as identified
 * by pointer
 *
 */

uint32_t user_location( struct process *proc, uint32_t pointer )
{
	struct allocated_memory *mem;
	
	  mem = proc->alloc;
	  while ( mem != NULL )
	  {
  	    if ( mem->virt == pointer) break;
	    mem = mem->next;
	  }

	  if ( mem == NULL ) return 0;

	  return mem->phys;
}


/*
 * Maps in a specified section of physical memory into userspace on request.
 */

unsigned int user_map( struct process *proc, uint32_t location, 
						unsigned int pages )
{
   struct allocated_memory *mem;
   uint32_t found;

   if ( pages == 0 ) return 0;
   if ( (location & (PAGE_SIZE-1)) != 0 ) return 0; // not aligned
   
	found = page_provide( proc->map,
						  location,
						  sK_BASE,
						  sK_CEILING,
						  pages,
						  USER_PAGE | READ_WRITE );
	
    if ( found != 0 )
    {
       mem = malloc( sizeof( struct allocated_memory ) );
       mem->pages = pages;
       mem->flags = UALLOC_NORELEASE;
       mem->shared = 0;
       mem->phys = location;
       mem->virt = found;
       mem->prev = NULL;
       mem->next = proc->alloc;
       mem->parent = proc;
       proc->alloc = mem;
       if ( mem->next != NULL ) mem->next->prev = mem;

    }

   
   return found;
}




/** This function has been implemented to correctly and cleanly
 * cut up the virtual destination so that they don't overlap
 * with each other.
 * 
 * The point of this is so that we can now make use of 
 * ualloc functions when mapping in process code. This 
 * makes management of the memory space a heck(!) of a lot
 * easier for me. Plus it provides one interface for me.
 *
 * \note Please note that this is not a fast function. It's just
 * used in the process set up so that we shouldn't really have
 * any problems anyway..
 *
 * \todo Anyway to improve this?
 */ 

int user_ensure( struct process *proc, uint32_t address, unsigned int pages )
{
	uint32_t index;
	uint32_t end = address + pages * PAGE_SIZE;
	uint32_t last_start = address;
	
	// Do a sequential search for open blocks and ualloc them.
	
	for ( index = address; index < end; index += PAGE_SIZE )
	{
		if ( page_present( proc->map, index ) != 0 ) continue;
			
		if ( index != last_start )
		{
			// Mapping last_start - index now.
			if (user_alloc_hard( proc, last_start, (index-last_start) / PAGE_SIZE ) != 0)
				return -1;
		}

		last_start = index + PAGE_SIZE;
	}


	if ( last_start < end )
	{
		// Mapping the left over last_start - index.
		if (user_alloc_hard( proc, last_start, (end - last_start) / PAGE_SIZE ) != 0)
			return -1;
	}
	

	return 0;
}








