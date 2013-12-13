#include <types.h>
#include <macros.h>
#include <dmesg.h>
#include <atomic.h>
#include <alloc.h>
#include <strings.h>
#include <configure.h>
#include <console.h>
#include <conio.h>
#include <smk.h>
#include <switching.h>
#include <process.h>
#include <paging.h>
#include <lfb.h>
#include <memory.h>
#include <rwlock.h>

//   The kernel is going to provide multiple pages to each process
//   for the console work.
//
//	1.  Each console has its own ID.
//	2.  Multiple processes can be attached to a single console.
//	3.  The whole console is mapped RW.


static struct console *consoles;  // The consoles.
static int active_console;		  // Active console ID.
static int console_count;		  // Number of consoles present.
static int screen_present;		  // Set to 1 if text-mode + screen

static spinlock console_lock = INIT_SPINLOCK;	// spinlock for consoles.


/// \todo Investigate replacing console list with hash table.


/*
   Initialized the consoles pointer and created INITIAL_CONSOLES
   amount of new, empty consoles.
 */
void init_consoles()
{
  int i;
  int id;
  char *buf;

    dmesg( "initializing consoles.\n" );

 
    active_console = -1;		// nothing active.

	consoles = (struct console*) 
		  malloc( sizeof(struct console) * INITIAL_CONSOLES ) ;
		
    console_count = INITIAL_CONSOLES;
    for ( i = 0; i < console_count; i++ )
      consoles[i].id = -1;
      		// Set all their ID's to empty.


    if ( using_lfb() != 1 )  screen_present = 1;
    			        else screen_present = 0;


	// Create a default console.
	dmesg( "creating the default console.\n" );
    id = new_console( 3 );

	
	#warning this is not portable.  how about an arch_new_console 
	
	// Clean out the default console;
    buf = (char*)consoles[id].data;
    for ( i = 0; i < (consoles[id].pages * PAGE_SIZE); i += 2 )
    {
        buf[i] = 0;
		buf[i + 1] = 7; 	// grey + white. Another colour?
    }

}

/*
   Locates an empty console, initializes it and returns the console ID.
*/

int new_console( int pages )
{
   int found;
   struct console *tmp = NULL;
   int i;

   	// request verification.
   if ( pages < 2 ) return -1;

   acquire_spinlock( & console_lock );
   
		// Find an empty console.
	   for ( found = 0; found < console_count; found++ )
	      if ( consoles[ found ].id == -1 )  break;
	
		// There's a valid console.
	    if ( found < console_count )
	     {
	        consoles[ found ].id = found;
			consoles[ found ].pages = pages;
			consoles[ found ].data = memory_alloc( pages );
			consoles[ found ].active_pid = -1;
			release_spinlock( & console_lock );
			return found;
	     }

		// There's no space left. We need to expand the list by x2.
	
		tmp = (struct console*) 
			  malloc( sizeof(struct console) * console_count * 2 );
	    
	    	// Save old data into new array.
	    for ( i = 0; i < console_count; i++ )
	     {
	       tmp[i].id    = consoles[i].id;
	       tmp[i].pages = consoles[i].pages;
	       tmp[i].data  = consoles[i].data;
	       tmp[i].active_pid = consoles[i].active_pid;
	     }
	
		// Clean the new data.
	    for ( i = console_count; i < console_count * 2; i++ )
	    	tmp[i].id = -1;

		// Free old array and replace it.
		free( consoles );
	
	    consoles = tmp;
	    console_count = console_count * 2;	// Keep track.
	
	release_spinlock( & console_lock );		// In preparation...

    return new_console( pages );	// should work 100% this time.
}


/*
   Creates a new console and copies the pages, located
   at *data, into the new console.
 */
int stage_console( void *data, int pages )
{
   int found;
   struct console *tmp = NULL;
   int i;


   	// request verification.
   if ( pages < 2 ) return -1;


	acquire_spinlock( & console_lock );		// In preparation...

		// Find an empty console.
	   for ( found = 0; found < console_count; found++ )
	      if ( consoles[ found ].id == -1 )  break;
	
		// There's a valid console.
	    if ( found < console_count )
	     {
	        consoles[ found ].id = found;
			consoles[ found ].pages = pages;
			consoles[ found ].data = memory_alloc( pages );
			consoles[ found ].active_pid = -1;
			memcpy( consoles[found].data, data, pages * PAGE_SIZE );
			release_spinlock( & console_lock );
			return found;
    	 }

		// There's no space left. We need to expand the list by x2.
	
		tmp = (struct console*) 
			    malloc( sizeof(struct console) * console_count * 2  ) ;
			
	    
	    
	    	// Save old data into new array.
	    for ( i = 0; i < console_count; i++ )
	     {
	       tmp[i].id    = consoles[i].id;
	       tmp[i].pages = consoles[i].pages;
	       tmp[i].data  = consoles[i].data;
	       tmp[i].active_pid  = consoles[i].active_pid;
	     }
	
		// Clean the new data.
	    for ( i = console_count; i < console_count * 2; i++ )
	    	tmp[i].id = -1;
	
		// Free old array and replace it.
		free( consoles ); 
	    consoles = tmp;
	
	    console_count = console_count * 2;	// Keep track.
	
	release_spinlock( & console_lock );

    return new_console( pages );	// should work 100% this time.
}


int delete_console( int id )
{
   if ( id < 0 ) return -1;
   if ( id >= console_count ) return -1;

   acquire_spinlock( & console_lock );
   
	   if ( consoles[id].id != id ) 
	   {
			release_spinlock( & console_lock );
			return -1;
		}

	   memory_free( consoles[id].pages, consoles[id].data );
	   consoles[id].id = -1;
			
	   
	release_spinlock( & console_lock );

   /// \todo  See if we can reduce the size of the console array.
   return 0;
}


// -------------------------------------

int console_size( int id )
{
	int size;
	// Sanity check.
   if ( id < 0 ) return -1;
   if ( id >= console_count ) return -1;

	acquire_spinlock( & console_lock );

		   if ( consoles[id].id != id ) size = -1;
		   else size =  consoles[id].pages * PAGE_SIZE;

	release_spinlock( & console_lock );
	
	return size;
}


int set_active_pid( int id, int pid )
{
	// Sanity check.
   if ( id < 0 ) return -1;
   if ( id >= console_count ) return -1;
   
	int ans = 0;
	   
   	acquire_spinlock( & console_lock );

	   if ( consoles[id].id == id ) 
   				consoles[id].active_pid = pid;
		   else 
				ans = -1;

	release_spinlock( & console_lock );
	
   return ans;
}

int get_active_pid( int id )
{
	// Sanity check.
   if ( id < 0 ) return -1;
   if ( id >= console_count ) return -1;

   int pid = -1;
   
   	acquire_spinlock( & console_lock );

	   if ( consoles[id].id == id ) 
   				pid = consoles[id].active_pid;

	release_spinlock( & console_lock );


   return pid;
}


// --------------------------------------------------------------


/*
	Basically just runs through all processes which are
	attached to the currently active console, unmaps the
	screen, saves the data back into their buffer
	and then maps the processes attached to the new
	active console onto the screen.

	Obviously only works in text mode.
 */

static int _target_id;

int set_active_callback( struct process *proc )
{
    // Out with the old...
    if ( (proc->console == active_console) || (proc->console == ALL_CONSOLES) )
	 if ( active_console >= 0 )
		arch_unmap_console( proc, consoles[active_console].data, consoles[active_console].pages );

	// ... and map in the new ones at the same time.
	if ( (proc->console == _target_id) ||  (proc->console == ALL_CONSOLES) )
	 arch_map_console( proc, consoles[_target_id].data, consoles[_target_id].pages );

	return 0;
}

int set_active_console( int id )
{
   acquire_spinlock( & console_lock );

   int insane = 0;
   
		// Sanity check.
	   if ( insane == 0 ) if ( active_console == id ) insane = 1;
	   if ( insane == 0 ) if ( id < 0 ) insane = 1;
	   if ( insane == 0 ) if ( id >= console_count ) insane = 1 ;
	   if ( insane == 0 ) if ( consoles[id].id != id ) insane = 1;
	   if ( insane == 0 ) if ( screen_present == 0 ) insane = 1;


	if ( insane == 1 )
	{
		release_spinlock( & console_lock );
		return -1;
	}

    // save current screen.
    if ( ( active_console >= 0 ) && ( active_console < console_count ) )
     if ( consoles[active_console].id == active_console )
     {
		arch_save_console( consoles[active_console].data, consoles[active_console].pages );
     }
	 
    // Now we have to unmap all the processes who are using this console.

	  _target_id = id;
	  
		  foreach_process( set_active_callback, WRITER );		// Wee...This is safe! hooray.


	  arch_restore_console( consoles[id].data, consoles[id].pages );
  	  active_console = id;


  release_spinlock( & console_lock );
  return 0;
}


// Unmap the shared regions and free the used memory.

int detach_console( struct process *p )
{
   int id;
   int ans;



   id = p->console;		// Get the console ID.
   if ( id == ALL_CONSOLES ) id = active_console;

	// Sanity check.
   if ( id < 0 ) return -1;
   if ( id >= console_count ) return -1;


   acquire_spinlock( & console_lock );
   
   	ans = -1;

	if ( consoles[id].id == id ) 
	{
		arch_unmap_console( p, consoles[id].data, consoles[id].pages );
	 	page_release( p->map, TO_UINTPTR(p->console_location), consoles[id].pages );
		
	   p->console = -1; 		
	   p->console_location = NULL;
	   ans = 0;
	}


   release_spinlock( & console_lock );

   return ans;
}



// Map in the shared data region.
// Also, map in the console itself.
//   If it happens to be the same console as the active console,
//   map in the screen instead.

int attach_console( struct process *p, int nid )
{
   int all;
   int id;
   int ans;


	// Support for attaching to all consoles.
    if ( nid == ALL_CONSOLES ) 
    {
    	all = 1;
		id = active_console;
    }
    else 
    {
    	all = 0;
		id = nid;
    }

	// Sanity check.
   if ( id < 0 ) return -1;
   if ( id >= console_count ) return -1;

   acquire_spinlock( & console_lock );

   ans = -1;
   
	   if ( consoles[id].id == id ) 
	   {
			// Set up the process.
		   if ( all == 1 ) p->console = ALL_CONSOLES;
		   		      else p->console = id;
	
		   	// Record it.
		   p->console_location = (void*)page_provide( p->map,
						 	     		TO_UINTPTR(consoles[id].data),
						 	     		app_BASE, sK_BASE,
							     		consoles[id].pages,
							     		USER_PAGE | READ_WRITE );
		
		
			// Just make sure the first page points to the screen
			// if it's the active console and we're in text-mode.
		
			if ( screen_present == 1 )			// text mode?
		 	  if ( id == active_console )
				arch_map_console( p, consoles[id].data, consoles[id].pages );


			ans = 0;
		}


   release_spinlock( & console_lock );

   return ans;
}





