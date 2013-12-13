#include <types.h>
#include <atomic.h>
#include <dmesg.h>
#include <macros.h>
#include <smk.h>
#include <smk_gc.h>
#include <process.h>
#include <waits.h>
#include <scheduler.h>
#include <switching.h>

/*
 *
 * This is going to be the garbage collector thread
 * which will clean up threads and processes when they
 * crash and finish.
 *
 * It will need to be signalled when there's a thread
 * or process to be cleaned up.
 * 
 */

// ******* STRUCTURE **********************************

struct smk_gc_signal
{
	int pid;
	int tid;
};


// ******* VARIABLES **********************************


#define	GC_REQUESTS				32

struct thread *smk_gc = NULL;

static struct smk_gc_signal gc_tasks[ GC_REQUESTS ];
static int gc_count;
static spinlock	gc_spinlock = INIT_SPINLOCK;


// ******* FORWARD DECLARATIONS ***********************

int gc();
int gc_cleanup( int pid, int tid );

int gc_thread( struct thread *t );
int gc_process( struct process *p );


// ****************************************************

int init_smk_gc()
{
	int i;
	int tid;
	
	dmesg("creating the garbage collector.\n");

	gc_count = 0;
	
	// Set up the smk garbage collector thread. ( see above )

	tid = new_thread( 1,  smk_process,
				    smk_process->threads,
				    "gc",
				    (uint32)gc,
				    1,
				    0,
				    0,
				    0,
				    0
				   );

	smk_gc = find_thread_with_id( smk_process, tid );
	set_thread_state( smk_gc, THREAD_SYSTEM );

		for ( i = 0; i < GC_REQUESTS; i++ )
		{
			gc_tasks[ i ].pid = -1;
			gc_tasks[ i ].tid = -1;
		}
	

	return 0;
}



// ****************************************************


/*
 * If a thread or process crashes, then this function is 
 * called to signal the garbage collector that there's
 * work to do.
 */
int signal_gc( int pid, int tid )
{
	int i;

	if ( check_spinlock(  & gc_spinlock ) != 0 ) return -1;

		// Too full?
		if ( gc_count == GC_REQUESTS ) return -1;

		// Already present? And find an open slot.
		
		int valid_spot = -1;
		
		for ( i = 0; i < GC_REQUESTS; i++ )
		{
			if ( (gc_tasks[i].pid == -1) && ( valid_spot < 0 )) valid_spot = i;
			if ( (gc_tasks[i].pid == pid) && (gc_tasks[i].tid == tid) ) return 0; 
		}

		// Add it in.
		gc_tasks[valid_spot].pid = pid;
		gc_tasks[valid_spot].tid = tid;
		atomic_inc( &gc_count );
	
		
	return 0;
}


int gc_needed()
{
	if ( check_spinlock(  & gc_spinlock ) != 0 ) return 0;

	if ( gc_count == 0 ) return -1;

	return 0;
}


int gc_check()
{
	return gc_count;
}

// ****************************************************



int gc()
{
	int i;

	while ( 1==1 )
	{
		acquire_spinlock( & gc_spinlock );


			for ( i = 0; i < GC_REQUESTS; i++ )
				if ( gc_tasks[i].pid != -1 )
				{
					if ( gc_cleanup( gc_tasks[i].pid, gc_tasks[i].tid ) == 0 )
					{
						gc_tasks[i].pid = -1;
						gc_tasks[i].tid = -1;
						atomic_dec( &gc_count );
					}
					else
					{
						dmesg("%!%s%i.%i\n", "Failure to clean!!!: ", 
									  	     gc_tasks[i].pid,
										     gc_tasks[i].tid );
					}
				}
		
		release_spinlock( & gc_spinlock );

		sched_yield();
	}


	return 0;
}



// ************ ACTUAL CLEAN UP ***********************

int gc_thread( struct thread *tr )
{
   clean_thread_waits( tr, tr->rc );	// wake up any sleepers
   delete_thread( tr );  				// free it's resources
   return 0;
}

int gc_process( struct process *p )
{
	clean_process_waits( p, p->rc );
	delete_process( p );
	return 0;
}

// ***********************************************

int gc_cleanup( int pid, int tid )
{
   struct process *proc		=	NULL;
   struct thread *tr		=	NULL;
   struct thread *tr_next	=	NULL;
   int clean				=	0;

   // Pure process deletion ------------------------
	   if ( tid == -1 )
	   {
		 proc = detach_process( pid );
		 if ( proc == NULL ) return -1;

		 tr = proc->threads;
		 
		 while ( tr != NULL )
		 {
			tr_next = tr->linear_next;

			set_thread_state( tr, THREAD_STOPPED );

			detach_thread( tr );
			gc_thread( tr );

			tr = tr_next;
		 }
	
		 return gc_process( proc );
	   }

   // Individual thread deletion -------------------


   proc = checkout_process( pid, WRITER ); 
   if ( proc == NULL ) return -1;


   
	// Locate the thread.
	   tr = find_thread_with_id( proc, tid );
	   if ( tr == NULL )
	   {
			commit_process( proc );
			return -1;
	   }


	   set_thread_state( tr, THREAD_STOPPED );	// Remove it from the scheduler list.

	   detach_thread( tr );						// Remove it from the process.
	   gc_thread( tr );							// Let it do it's job.

	
	   if ( proc->threads == NULL ) clean = 1;

   commit_process( proc );
	
	// Clean the process if all done -------------
   
	if ( clean == 1 )
	{
		proc = detach_process( pid );
		if ( proc == NULL ) return -1;	// Really freaky.

		return gc_process( proc );
	}

	// Done....................
	   
	return 0;
}





