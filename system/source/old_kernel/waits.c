#include <types.h>
#include <macros.h>
#include <atomic.h>
#include <configure.h>
#include <interrupts.h>
#include <waits.h>
#include <alloc.h>
#include <scheduler.h>
#include <interrupts.h>
#include <switching.h>
#include <process.h>
#include <rwlock.h>
#include <conio.h>



int begin_wait_process( int pid, int *rc )
{
  int success = -1;
  struct wait_info *nw 	= NULL;
  struct process *proc;

  proc = checkout_process( pid, WRITER );
  if ( proc == NULL ) return -1;


	nw = (struct wait_info*)malloc( sizeof(struct wait_info) ); 
	
	  nw->next = NULL;
	  nw->prev = NULL;
	  
		  nw->pid 		= current_pid();
		  nw->tid	 	= current_tid();
		  nw->success 	= -1;	// Assume failure from the very beginning.
		  nw->rc 		= -1;

	// Now we insert it into the wait list.
	  if ( proc->waits != NULL ) proc->waits->prev = nw;
	  nw->next = proc->waits;
	  proc->waits = nw;

	// -----------------------------
	commit_process( proc );

	// ------  Now we go to sleep -------------
		proc = checkout_process( current_pid(), WRITER );
		ASSERT( proc != NULL );

		current_thread()->active_wait = nw;		// Save our active wait.
	
		   	disable_interrupts();
			   atomic_dec( &(proc->kernel_threads) );
			   set_thread_state( current_thread(), THREAD_WAITING );  
			   commit_process( proc );
			enable_interrupts();

			sched_yield();	
			
			atomic_inc( &(proc->kernel_threads) );	// Secure this thread.
		
	// Get our process back.
  	proc = checkout_process( current_pid(), WRITER );
	ASSERT( proc != NULL );

		current_thread()->active_wait = NULL;
	
	commit_process( proc );

			
	// We're back. Return the correct info.
	 *rc = nw->rc;
	 success = nw->success;

	 // nw should have been unlinked by the scheduler.
	 // waiter should have active_wait cleared by the
	 // scheduler as well.
	 // we just need to delete it.

	free( nw );

	/// \todo active_waits for threads.

  return success;
}


int begin_wait_thread( int pid, int tid, int *rc )
{
  int success = -1;
  int size;
  struct wait_info *nw 	= NULL;
  struct process *proc;
  struct thread *tr;

  ASSERT( pid == current_pid() );
  
  proc = checkout_process( pid, WRITER );
  ASSERT( proc != NULL );

  	tr = find_thread_with_id( proc, tid );
	if ( tr == NULL )
	{
		commit_process( proc );
		return -1;
	}


	// --------------------------------
	
	size = sizeof(struct wait_info);
	nw = (struct wait_info*)malloc( size ); 
	
	  nw->next = NULL;
	  nw->prev = NULL;
	  
		  nw->pid 		= current_pid();
		  nw->tid	 	= current_tid();
		  nw->success 	= -1;	// Assume failure from the very beginning.
		  nw->rc 		= -1;

	current_thread()->active_wait = nw;	// Set our active wait information.
		  
	// Now we insert it into the wait list.
	  if ( tr->waits != NULL ) tr->waits->prev = nw;
	  nw->next = tr->waits;
	  tr->waits = nw;

	// -----------------------------
	commit_process( proc );

	// ------  Now we go to sleep -------------
		proc = checkout_process( current_pid(), WRITER );
		if ( proc == NULL )
		{
			/// \todo freak out and handle stuff properly
			return -1;
		}
	
		   	disable_interrupts();
			   atomic_dec( &(proc->kernel_threads) );
			   set_thread_state( current_thread(), THREAD_WAITING );  
			   commit_process( proc );
			enable_interrupts();
			
			sched_yield();		// Release!	
			
						// Secure ourselves.
			atomic_inc( &(proc->kernel_threads) );

	// Get our process back.
  	proc = checkout_process( current_pid(), WRITER );
	if ( proc == NULL ) return -1;
			
		current_thread()->active_wait = NULL;

	commit_process( proc );
	
		// We're back. Return the correct info.
		 *rc = nw->rc;
		 success = nw->success;
	
		 // nw should have been unlinked by the scheduler.
		 // waiter should have active_wait cleared by the
		 // scheduler as well.
		 // we just need to delete it.
	
		free( nw );

  return success;

}



// ----------------------------------------------------------------

int clean_general( struct process *owner, struct wait_info *wait, int rc )
{
  struct process *proc;
  struct thread *tr;
  int count = 0;
  int clean = 0;
  
  struct wait_info *tmp = wait;
  struct wait_info *old = NULL;
    
    while ( tmp != NULL )
    {
      tmp->success = 0;
      tmp->rc = rc;

	  clean = 0;

	  // Make sure that it's not already locked.
	  if ( owner->pid == tmp->pid ) proc = owner;
	  else proc = checkout_process( tmp->pid, WRITER );
	  
		  if ( proc != NULL )
		  {
			tr = find_thread_with_id( proc, tmp->tid );
			if ( tr != NULL )
			{
				set_thread_state( tr, THREAD_RUNNING );
			}
			else clean = 1;	// Nothing waiting on this wait. Need to clean it.
				  
			if ( proc != owner ) commit_process( proc );

		  }
		  else clean = 1; // Nothing waiting on this wait. Need to clean it.
	  

	  old = tmp;
      tmp = tmp->next;
		
		  // Clean the wait if the waiter was not found.
	  	if ( clean == 1 ) free( old );
    }

  return count;
}

// --------------------------------------------------------

int clean_process_waits( struct process *proc, int rc )
{
  struct wait_info *wait = NULL;

    wait = proc->waits;
    proc->waits = NULL;
   
    return clean_general( proc, wait, rc );
}

int clean_thread_waits( struct thread *tr, int rc )
{
  struct wait_info *wait = NULL;

    wait = tr->waits;
    tr->waits = NULL;
   
    return clean_general( tr->process, wait, rc );
}

