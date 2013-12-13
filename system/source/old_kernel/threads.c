#include <configure.h>
#include <conio.h>
#include <strings.h>
#include <atomic.h>
#include <macros.h>
#include <memory.h>
#include <alloc.h>
#include <paging.h>
#include <threads.h>
#include <process.h>
#include <syscalls.h>
#include <switching.h>
#include <scheduler.h>
#include <pulse.h>
#include <time.h>
#include <messages.h>
#include <htable.h>
#include <interrupts.h>


char *states[] = 
{
  "THREAD_SYSTEM",
  "THREAD_RUNNING",
  "THREAD_SUSPENDED",
  "THREAD_DORMANT",
  "THREAD_STOPPED",
  "THREAD_SEMAPHORE",
  "THREAD_SLEEPING",
  "THREAD_WAITING",
  "THREAD_FINISHED",
  "THREAD_CRASHED",
  "THREAD_IRQ"
};






// ---- architecture calls ----------

int arch_new_thread( struct thread *t, uint32 type,
						   uint32 entry,
						   uint32 data1,
						   uint32 data2,
						   uint32 data3,
						   uint32 data4 );



int arch_delete_thread( struct thread *t );

// -----------------------------------------------
//  htable key and compare functions

int threads_key( void *td )
{
	struct thread *t = (struct thread*)td;
	int ans = t->tid * 100;
	if ( ans < 0 ) ans = -ans;
	return ans;
}

int threads_compare( void *ad, void *bd )
{
	struct thread *a = (struct thread*)ad;
	struct thread *b = (struct thread*)bd;

	if ( (b->tid != -1) )
	{
		if ( a->tid < b->tid ) return -1;
		if ( a->tid > b->tid ) return 1;
		return 0;
	}
	
	return strcmp( a->name, b->name );
}


// ----------------------------------

int	thread_length = 0;

int new_thread( int type,
	           struct process *proc,
	           struct thread *parent, 
	           const char name[OS_NAME_LENGTH],
			   uint32 entry,
			   uint32 priority, 
			   uint32 data1,
			   uint32 data2,
			   uint32 data3,
			   uint32 data4 )
{
	struct thread *t;
	int i;

		
	t = malloc( sizeof(struct thread) );
	if ( t == NULL ) return -1;

	memcpy( t->name, name, OS_NAME_LENGTH );

	     
		t->tid = proc->thread_count++;
	
		t->sleep_seconds = 0;
		t->sleep_milliseconds = 0;
	
		  
		t->priority = priority;
		t->state = THREAD_SUSPENDED;
		t->rc = -1;
	
		t->tls = NULL;			// to be set later by userland
	

	// ------------
   
		t->process = proc;
		t->parent = parent;
		t->child = NULL;
		t->sibling_next = NULL;
		t->sibling_prev = NULL;
		t->linear_next = NULL;
		t->linear_prev = NULL;
	

		for ( i = 0; i < MAX_PORTS; i++ ) 
			t->ports[i] = -1;
	
		t->last_port_pulse = -1;
		t->last_port_message = -1;
	
		t->waits = NULL;
		t->active_wait = NULL;
		
	
		for ( i = 0; i < TOTAL_SYSCALLS; i++ )
			{
			  if ( parent == NULL )	
			  {
				 if ( proc->threads != NULL )
				   t->capabilities[i] = proc->threads->capabilities[i];
				 else
				   t->capabilities[i] = 0;
			  }
			  else
			  {
			     t->capabilities[i] = parent->capabilities[i];
			  }
	
			}


	t->data = NULL;
	arch_new_thread( t, type, entry, data1, data2, data3, data4 );
	attach_thread( parent, t );

	return t->tid;
}


/*
 * attach_thread - insert the thread into the process's thread
 *                 list. It inserts it as both a linear linked
 *                 list and as a parent -> many children tree.
 *                 Also a hash table.
 *
 */

void attach_thread( struct thread *parent, struct thread *t )
{
	struct process *proc;
	struct thread *temp;
	//struct thread *tmp;

	proc = t->process;

/*
*/
	
	atomic_inc(  & thread_length );

	// cleanse our pointers first - always a good idea for ANYTHING in c.
		t->linear_next = NULL;
		t->linear_prev = NULL;
		t->sibling_next = NULL;
		t->sibling_prev = NULL;

	// hash table insertion -------------------------
		htable_insert( proc->threads_htable, t );
	
	// Insert linearly into the process
	     temp = t->process->threads;
		 if ( temp == NULL )
		 {
		    t->process->threads = t;
		 }
		 else
		 {
		    while ( temp->linear_next != NULL ) temp = temp->linear_next;
		    t->linear_prev = temp;
		    temp->linear_next = t;
		 }

	// familial insert
	 if ( parent != NULL )
	 {
	   t->sibling_next = parent->child;
	   t->sibling_prev = NULL;
	   if ( parent->child != NULL ) parent->child->sibling_prev = t;
	   parent->child = t;
	 }

/*
*/
	

}

/*
 * detach_thread - removes a thread from the process's thread
 *                 tree/list and gives all of the thread's children
 *                 to the parent, if it exists.
 *
 */

void detach_thread( struct thread *t )
{
	struct thread *tmp;

	atomic_dec(  & thread_length );

		// K.. this is a little bit dangerous. Must drink coffee first.

/*
*/
	
	
	// hash table insertion -------------------------
	htable_remove( t->process->threads_htable, t );
	
	// linear delete ------------------------------------
	if ( t->linear_next != NULL ) t->linear_next->linear_prev = t->linear_prev;
	if ( t->linear_prev != NULL ) t->linear_prev->linear_next = t->linear_next;

	// familial delete ----------------------------------
	if ( t->sibling_next != NULL ) 
		 t->sibling_next->sibling_prev = t->sibling_prev;

	if ( t->sibling_prev != NULL ) 
  	         t->sibling_prev->sibling_next = t->sibling_next;
	
			// Remove from the parent, if it the root child
			if ( t->parent != NULL )
			 if ( t->parent->child == t ) 
			   t->parent->child = t->sibling_next;

			// Remove from the process if it's the main thread.
			if ( t->parent == NULL )
			 if ( t->process->threads == t )
				t->process->threads = t->sibling_next;
			
	// ---------------------------------------------------
	
			// Now we're a detached thread with all it's children.
			// The parent no longer knows about us.
			// The siblings no longer know about us.
			// The process no longer knows about us.
			
	// If we're one of the main threads, our children must be linked to
	// the process.
	
		if ( (t->parent == NULL) && (t->child != NULL) )
		{
			if ( t->process->threads == NULL )
			{
				t->process->threads = t->child;
			}
			else
			{
				tmp = t->process->threads;
				while ( tmp->sibling_next != NULL ) tmp = tmp->sibling_next;
				tmp->sibling_next = t->child;
				t->child->sibling_prev = tmp;
			}
		
				// Let the children know that they are main threads now
			tmp = t->child;
			while ( tmp != NULL )
			{
				tmp->parent = NULL;
				tmp = tmp->sibling_next;
			}
		}
		else 
			 if ( (t->parent != NULL) && (t->child != NULL) )
			 {
					// If we're not a main thread, our children must
					// move to the parent's list.

					 
				if ( t->parent->child == NULL )
				{
					t->parent->child = t->child;
				}
				else
				{
				    tmp = t->parent->child;
					while ( tmp->sibling_next != NULL ) tmp = tmp->sibling_next;
					tmp->sibling_next = t->child;
					t->child->sibling_prev = tmp;
				}

				tmp = t->child;
				while ( tmp != NULL )
				{
					tmp->parent = t->parent;
					tmp = tmp->sibling_next;
				}
				
			 }
				
			
    // PHEW.....
	

/*
*/

}





	

void set_thread_state( struct thread *t, uint32 state )
{
	int remove = 0;
	int insert = 0;

	
	if ( t->state == state ) return;			// POINTLESS

	
	switch (t->state)
	{
		case THREAD_RUNNING:  
		case THREAD_WAITING:
		case THREAD_DORMANT:
		case THREAD_SLEEPING:
		case THREAD_SEMAPHORE:
		case THREAD_CRASHED:
		case THREAD_FINISHED:
				remove = 1;
				break;
		default:
			remove = 0;
			break;
	}
	
	
	   t->state = state;


	switch (t->state)
	{
		case THREAD_RUNNING:  
		case THREAD_WAITING:
		case THREAD_DORMANT:
		case THREAD_SLEEPING:
		case THREAD_SEMAPHORE:
		case THREAD_CRASHED:
		case THREAD_FINISHED:
		case THREAD_SUSPENDED:
				insert = 1;
				break;
		default:
			insert = 0;
			break;
	}
	

	if ( remove == insert ) return;

	if ( remove == 1 ) sched_remove( t );
	if ( insert == 1 ) sched_insert( t );
}


struct thread *find_thread_with_id( struct process *p, int id )
{
	struct thread temp;
				  temp.tid = id;
				  temp.name[0] = 0;
	
	return (struct thread*)htable_retrieve( p->threads_htable, &temp );
}


struct thread *find_thread_with_name( struct process *p, const char name[OS_NAME_LENGTH] )
{
	struct thread temp;
				  temp.tid = -1;
				  memcpy( temp.name, name, OS_NAME_LENGTH );
	
	return (struct thread*)htable_retrieve( p->threads_htable, &temp );
}

// -------------------------------------------


// Works on current_pid() + current_tid()

int go_dormant( int timeout )
{
	struct process *proc;
	struct thread *tr;
  	time_t seconds,milliseconds;
	int answer;
	
	// First check if messages are waiting...
	proc = checkout_process( current_pid(), WRITER );
	ASSERT( proc != NULL );

		// Allowed to go dormant?
	    tr = current_thread();
	
		if ( (pulses_waiting(tr) > 0) || (messages_waiting(tr) > 0) )
		{
			commit_process( proc );
			return 0;
		}
			
	// We're allowed to go dormant.
	
	    if ( timeout != 0 )
	    {
			// THIS DORMANCY HAS A TIMEOUT.
			system_time( &seconds, &milliseconds );
			milliseconds += timeout;
			seconds += (milliseconds / 1000);
			milliseconds = milliseconds % 1000;
		
			tr->sleep_milliseconds = milliseconds; 
			tr->sleep_seconds = seconds;
	    }
	    else
	    {
			tr->sleep_milliseconds = 0;
	    	tr->sleep_seconds = 0;
	    }
	
		disable_interrupts();
			atomic_dec( &(proc->kernel_threads) );
		    set_thread_state( tr, THREAD_DORMANT );
			commit_process( proc );
		enable_interrupts();

		    sched_yield();	// we go dormant here...
				
				
	   	answer = 0;
	   	if ( timeout != 0 ) if ( tr->sleep_seconds != 0 ) answer = -1;

			atomic_inc( &(proc->kernel_threads) );

	return answer;
}


// -------------------------------------------
// -


int delete_thread( struct thread *t )
{
   arch_delete_thread( t );
   free( t );
   return 0;
}


// ------------------------------------------------------------------






