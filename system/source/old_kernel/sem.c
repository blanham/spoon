#include <macros.h>
#include <dmesg.h>
#include <atomic.h>
#include <configure.h>
#include <alloc.h>
#include <types.h>
#include <sem.h>
#include <conio.h>

#include <rwlock.h>
#include <process.h>
#include <interrupts.h>

#include <scheduler.h>



struct semaphore global_sems[ GLOBAL_SEM_COUNT ];
spinlock	global_sems_lock = INIT_SPINLOCK;


void init_semaphores()
{
	int i;

	dmesg( "initialising the semaphore structures.\n");

	for ( i = 0; i < GLOBAL_SEM_COUNT; i++)
		global_sems[i].sem_id = -1;

}


// ********************************************************


int create_global_semaphore( int pid, int capacity )
{
	int i;
	int sem_id = -1;

	if ( capacity < 0 ) return -1;

	acquire_spinlock( & global_sems_lock );
	
		for (  i = 0; i < GLOBAL_SEM_COUNT; i++)
		 if ( global_sems[i].sem_id == -1 )
		 {
		 	global_sems[i].sem_id 	= i;
		 	global_sems[i].capacity = capacity;
		 	global_sems[i].count 	= 0;
		 	global_sems[i].pid 		= pid;
		 	global_sems[i].waiting_list 	= NULL;
			sem_id = i;
			break;
		 }

	release_spinlock( & global_sems_lock );

	return sem_id;
}

//  destroy actually waits until everyone is finished with the 
//  semaphore before it's destroyed.

int destroy_global_semaphore( int pid, int sem_id )
{
	struct sem_link *sl		= NULL;
	struct sem_link *tmp	= NULL;
	struct process *proc	= NULL;
	struct thread *tr		= NULL;

	if ( sem_id < 0 ) return -1;
	if ( sem_id >= GLOBAL_SEM_COUNT ) return -1;
	
	acquire_spinlock( & global_sems_lock );

		if ( ( global_sems[ sem_id ].sem_id != sem_id ) ||
			 ( global_sems[ sem_id ].pid 	!= pid ) )
		{
			// Invalid or not allowed.
			release_spinlock( & global_sems_lock );
			return -1;
		}
	
		// Tell the waiting guys to go away.
		sl = global_sems[ sem_id ].waiting_list;
		while ( sl != NULL )
		{
			tmp = sl;
			
			proc = checkout_process( sl->pid, WRITER );
			if ( proc != NULL ) 
			{
				tr = find_thread_with_id( proc, sl->tid );
				if ( tr != NULL ) 
						set_thread_state( tr, THREAD_RUNNING );
				commit_process( proc );
			}
			sl = sl->next;
			free( tmp );
		}

	 	global_sems[ sem_id ].waiting_list = NULL;
		global_sems[ sem_id ].sem_id 	= -1;	// DELETED!

	release_spinlock( & global_sems_lock );
	return 0;
}




int create_local_semaphore( struct process *proc, int capacity )
{
	int i;
	int sem_id = -1;

	if ( capacity < 0 ) return -1;

	acquire_spinlock( & (proc->sems_lock) );

		for (  i = 0; i < LOCAL_SEM_COUNT; i++)
		 if ( proc->sems[i].sem_id == -1 )
		 {
		 	proc->sems[i].sem_id 	= i;
		 	proc->sems[i].capacity 	= capacity;
		 	proc->sems[i].count 	= 0;
		 	proc->sems[i].pid = proc->pid;	// Hmm, redundant. tid rather?
		 	proc->sems[i].waiting_list = NULL;
			sem_id = i;
			break;
		 }

	release_spinlock( & (proc->sems_lock) );
		
	return sem_id;
}



int destroy_local_semaphore( struct process *proc, int sem_id )
{
	struct sem_link *sl		= NULL;
	struct sem_link *tmp	= NULL;
	struct thread *tr		= NULL;

	if ( sem_id < 0 ) return -1;
	if ( sem_id >= LOCAL_SEM_COUNT ) return -1;
	
	acquire_spinlock( & (proc->sems_lock) );
		
		if ( proc->sems[ sem_id ].sem_id != sem_id ) 
		{
			release_spinlock( & (proc->sems_lock) );
			return -1;
		}
		

		// Tell the waiting guys to go away.
		sl = proc->sems[ sem_id ].waiting_list;
		while ( sl != NULL )
		{
			tmp = sl;

			tr = find_thread_with_id( proc, sl->tid );
			if ( tr != NULL )
					set_thread_state( tr, THREAD_RUNNING );

			sl = sl->next;
			free( tmp );
		}

		proc->sems[ sem_id ].waiting_list = NULL;
		proc->sems[ sem_id ].sem_id 	= -1; // DELETED!

	release_spinlock( & (proc->sems_lock) );
	return 0;
}





int wait_global_semaphore( struct thread* tr, int sem_id )
{
	struct process *proc	= NULL;
	struct sem_link *sl		= NULL;
	struct sem_link *tmp	= NULL;

	if ( sem_id < 0 ) return -1;
	if ( sem_id >= GLOBAL_SEM_COUNT ) return -1;
	
	proc = tr->process;

	acquire_spinlock( & global_sems_lock );
			
		// Valid semaphore?
		if ( global_sems[ sem_id ].sem_id != sem_id ) 
		{
			release_spinlock( & global_sems_lock );
			return -1;
		}

		// Insert this thread into the waiting list and wait, if required.
		if ( global_sems[ sem_id ].count == global_sems[ sem_id ].capacity )
		{
			sl = (struct sem_link*)malloc(  sizeof( struct sem_link ) );
			sl->tid 	= tr->tid;
			sl->pid 	= proc->pid;
			sl->next 	= NULL;
	
			if ( global_sems[ sem_id ].waiting_list == NULL ) 
					global_sems[ sem_id ].waiting_list = sl;
			else
			{
			  tmp = global_sems[ sem_id ].waiting_list;
			  while ( tmp->next != NULL ) tmp = tmp->next;
			  tmp->next = sl;
			}
	
			// Now wait...

			disable_interrupts();
				set_thread_state( tr, THREAD_SEMAPHORE );
				release_spinlock( & global_sems_lock );
			enable_interrupts();

				sched_yield();	// wait proper.. won't come back for a while.

			acquire_spinlock( & global_sems_lock );

			if ( global_sems[ sem_id ].sem_id != sem_id )
			{
				release_spinlock( & global_sems_lock );
				return -1;		// Must have died... damn.
			}
		}

	global_sems[ sem_id ].count += 1;

	release_spinlock( & global_sems_lock );

	return 0;
}


int wait_local_semaphore( struct thread* tr, int sem_id )
{
	struct process *proc	= NULL;
	struct sem_link *sl		= NULL;
	struct sem_link *tmp	= NULL;

	if ( sem_id < 0 ) return -1;
	if ( sem_id >= LOCAL_SEM_COUNT ) return -1;
	
	proc = tr->process;

	acquire_spinlock( & (proc->sems_lock) );
			
		// Valid semaphore?
		if ( proc->sems[ sem_id ].sem_id != sem_id ) 
		{
			release_spinlock( & (proc->sems_lock) );
			return -1;
		}

		// Insert this thread into the waiting list and wait, if required.
		if ( proc->sems[ sem_id ].count == proc->sems[ sem_id ].capacity )
		{
			sl = (struct sem_link*)malloc(  sizeof( struct sem_link ) );
			sl->tid 	= tr->tid;
			sl->pid 	= proc->pid;
			sl->next 	= NULL;
	
			if ( proc->sems[ sem_id ].waiting_list == NULL ) 
					proc->sems[ sem_id ].waiting_list = sl;
			else
			{
			  tmp = proc->sems[ sem_id ].waiting_list;
			  while ( tmp->next != NULL ) tmp = tmp->next;
			  tmp->next = sl;
			}
	
			// Now wait...

			disable_interrupts();
				set_thread_state( tr, THREAD_SEMAPHORE );
				release_spinlock( & (proc->sems_lock) );
			enable_interrupts();

				sched_yield();	// wait proper.. won't come back for a while.

			acquire_spinlock( & (proc->sems_lock) );

			if ( proc->sems[ sem_id ].sem_id != sem_id )
			{
				release_spinlock( & (proc->sems_lock) );
				return -1;		// Must have died... damn.
			}
		}

	proc->sems[ sem_id ].count += 1;

	release_spinlock( & (proc->sems_lock) );

	return 0;
}






int signal_global_semaphore( struct thread* tr, int sem_id )
{
	struct sem_link *sl;
	struct process *proc;
	struct thread *target;

	acquire_spinlock( & global_sems_lock );
		
		if ( global_sems[ sem_id ].sem_id != sem_id ) 
		{
			release_spinlock( & global_sems_lock );
			return -1;
		}
		
		global_sems[ sem_id ].count -= 1;
			
			// wake up any waiting threads
			sl = global_sems[ sem_id ].waiting_list;	
			if ( sl != NULL )
			{
				proc = checkout_process( sl->pid, WRITER );
				if ( proc != NULL )
				{
					target = find_thread_with_id( proc, sl->tid );
					if ( target != NULL )
						set_thread_state( target, THREAD_RUNNING );

					commit_process( proc );
				}
				global_sems[ sem_id ].waiting_list = sl->next;
				free( sl );
			}

	release_spinlock( & ( global_sems_lock ) );
	return 0;
}



int signal_local_semaphore( struct thread* tr, int sem_id )
{
	struct sem_link *sl;
	struct thread *target;
	struct process *proc;

	proc = tr->process;
	
	acquire_spinlock( & (proc->sems_lock) );
		
		if ( proc->sems[ sem_id ].sem_id != sem_id ) 
		{
			release_spinlock( & (proc->sems_lock) );
			return -1;
		}
		
		proc->sems[ sem_id ].count -= 1;
			
			// wake up any waiting threads
			sl = proc->sems[ sem_id ].waiting_list;	
			if ( sl != NULL )
			{
				target = find_thread_with_id( tr->process, sl->tid );
				if ( target != NULL )
						set_thread_state( target, THREAD_RUNNING );

				proc->sems[ sem_id ].waiting_list = sl->next;
				free( sl );
			}

	release_spinlock( & ( proc->sems_lock ) );
	return 0;
}




