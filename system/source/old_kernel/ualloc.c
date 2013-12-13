#include <configure.h>
#include <macros.h>
#include <types.h>
#include <process.h>
#include <paging.h>
#include <memory.h>
#include <alloc.h>
#include <conio.h>
#include <ualloc.h>

#include <smk.h>


uintptr user_alloc( struct process *proc, uint32 pages)
{
   struct allocated_memory *mem;
   void *location;
   uint32 found;
   
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
						  TO_UINTPTR( location ),	
						  app_BASE,
						  sK_BASE,
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
    mem->phys = TO_UINTPTR( location );
    mem->virt = found;
    mem->prev = NULL;
    mem->next = proc->alloc;
    mem->parent = proc;
    proc->alloc = mem;
    if ( mem->next != NULL ) mem->next->prev = mem;

   
   return found;
}

uintptr user_free( struct process *proc, uint32 address )
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

int32 user_size( struct process *proc, uint32 pointer )
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

uint32 user_location( struct process *proc, uint32 pointer )
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
 * Maps in a specified section of memory into userspace on request.
 */

uintptr user_map( struct process *proc, uintptr location, uint32 pages )
{
   struct allocated_memory *mem;
   uint32 found;

   if ( pages == 0 ) return 0;
   if ( (location & (PAGE_SIZE-1)) != 0 ) return 0; // not aligned
   
	found = page_provide( proc->map,
						  location,
						  app_BASE,
						  sK_BASE,
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




