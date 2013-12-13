#include <configure.h>
#include <macros.h>
#include <alloc.h>
#include <types.h>
#include <lengths.h>
#include <memory.h>
#include <paging.h>
#include <def_thread.h>
#include <process.h>

#include "eflags.h"
#include "gdt.h"
#include "thread_data.h"
#include "paging.h"

void pushl( struct thread *t, uint32 value )
{
	uint32 *mem;
	struct thread_data *data = (struct thread_data*)(t->data);
	data->stack = data->stack - 4;
	mem = (uint32*)(data->stack);
	mem[0] = value;
}




int arch_new_thread( struct thread *t, int type, 
						   uint32 entry,
						   uint32 data1,
						   uint32 data2,
						   uint32 data3,
						   uint32 data4 )
{
	struct thread_data *data;

		t->data = malloc( sizeof( struct thread_data ) );
		data = t->data;
		
        if ( t->parent == NULL ) 
		{
			data->stack_pages = sK_PAGES_PROCESS;
 	 	    data->stack_user  = sK_CEILING;
		}
		else
		{
		    data->stack_pages = sK_PAGES_THREAD;

			data->stack_user  = page_find_free( t->process->map, 
											    sK_BASE,
												sK_CEILING,
												sK_PAGES_THREAD + 1 );
			
			data->stack_user += (sK_PAGES_THREAD + 1) * PAGE_SIZE;
		}


		// We're leaving a guard page at the bottom (and,
		// therefore, the top) of every thread stack.

		
  	        page_map_in( t->process->map , 
	 	          		 data->stack_user - (data->stack_pages * PAGE_SIZE),
	 	          		 data->stack_user,
				     	 data->stack_pages, 
		 	 	         PAGE_USER | PAGE_RDWR );


		data->stack_kernel = (uint32) memory_alloc( sK_PAGES_KERNEL );
		data->stack_kernel += sK_PAGES_KERNEL * PAGE_SIZE;
	     
	data->math_state = -1;
	data->math = memory_alloc( 1 );
	data->stack = data->stack_kernel;

	// ------------
   
	if ( type == 1 )
	{
		// kernel thread
		pushl( t, system_DS() );
		pushl( t, data->stack_user );
		pushl( t, EFLAG_BASE | EFLAG_INTERRUPT  );
		pushl( t, system_CS() );
	}
	else
	{
		// user thread
		pushl( t, user_DS() | 3 );
		pushl( t, data->stack_user );
		pushl( t, EFLAG_BASE | EFLAG_INTERRUPT  );
		pushl( t, user_CS() | 3 );
	}

	pushl( t, entry );

	pushl( t, user_DS() | 3 );
	pushl( t, user_DS() | 3 );
	pushl( t, user_DS() | 3 );
	pushl( t, user_DS() | 3 );
	pushl( t, 0 );
	pushl( t, 0 );
	pushl( t, 0 );

	pushl( t, data4 );
	pushl( t, data3 );
	pushl( t, data2 );
	pushl( t, data1 );

	return 0;
}





int arch_delete_thread( struct thread* t )
{
	struct thread_data *data = (struct thread_data*)t->data;


	page_map_out( t->process->map, 
				  (data->stack_user - (data->stack_pages * PAGE_SIZE)),
				  data->stack_pages );
	
	memory_free( sK_PAGES_KERNEL, (void*)(data->stack_kernel - sK_PAGES_KERNEL * PAGE_SIZE) );
	
	memory_free( 1, data->math  );
	free( data );
	t->data = NULL;
	
	return 0;
}

