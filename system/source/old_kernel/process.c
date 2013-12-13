#include <types.h>
#include <configure.h>
#include <alloc.h>
#include <strings.h>
#include <macros.h>
#include <paging.h>
#include <atomic.h>

#include <sem.h>
#include <process.h>
#include <env.h>
#include <shmem.h>

#include <htable.h>
#include <rwlock.h>


// Latest comments:  safe to use without locking.
// 					 do not use in a non-interruptable state.


/// \todo Make pids unique (based on time). They should never, ever re-occur.


char *process_states[] = 
{
  "SYSTEM",
  "RUNNING",
  "STOPPED",
  "FINISHING",
  "FINISHED",
  "CRASHING",
  "CRASHED"
};





// --------- architecture defines --------


int  arch_new_process( struct process* p );
int  arch_delete_process( struct process* p );

// --------------------------------------

//    hash table key and compare functions.

int ptable_key( void *d )
{
	struct process *p = (struct process*)d;
	int ans = p->pid * 100;
	if ( ans < 0 ) ans = -ans;
	return ans;
}

int ptable_compare( void* ad, void* bd )
{
	struct process *a = (struct process*)ad;
	struct process *b = (struct process*)bd;

	if ( (b->pid != -1) )
	{
		if ( a->pid == b->pid ) return 0; 
		if ( a->pid < b->pid ) return -1;
		return 1;
	}
	
	return strcmp( a->name, b->name );
}

// --------------------------------------

struct rwlock *process_rwlock		= NULL;		// RW lock for the process list.

struct process *process_list 		= NULL;		// Linear linked list.
struct hash_table *process_htable	= NULL;		// Hash table.


int process_counter;							// Keeps the latest PID.
int process_length;								// Keeps the total number of PIDs.

// -----------------------------------------------------------

void init_processes()
{
	process_rwlock	  = init_rwlock();
	process_list      = NULL;
	process_counter   = 1;
	process_length 	  = 0;

	process_htable = init_htable( 128, 75,
								 	 ptable_key,
								 	 ptable_compare );
};



// -----------------------------------------------------------

int set_process_state( struct process *proc, uint32 state )
{  
   proc->state = state;
   return 0;
}


// ----------------------------------------------------------


#include <conio.h>

/** This is an easy method to parse the process list while keeping
 * the whole thing locked, safe, sane, etc.
 *
 * If the callback function returns anything else besides 0, then
 * the callback processing loop ends.
 */
int foreach_process( process_callback callback, unsigned int flags )
{
	int ans = 0;
	struct process *p;
	
	if ( (flags == READER) ) rwlock_get_read_access( process_rwlock );
	if ( (flags == WRITER) ) rwlock_get_write_access( process_rwlock );

		p = process_list;
		while ( p != NULL )
		{
			ans = callback( p );
			if ( ans != 0 ) break;
			p = p->linear_next;
		}
		
	
	rwlock_release( process_rwlock );
	
	return ans;
}



// ----------------------------------------------------------


int find_process( const char name[OS_NAME_LENGTH] )
{
	struct process *p;
	struct process temp;
		   		   temp.pid = -1;
				   
		   memcpy( temp.name, name, OS_NAME_LENGTH );

	int pid = -1;
	
		   	// Hash tables still have the potential to be faster than a linked list.
			// depending on where the key first takes us.
	rwlock_get_read_access( process_rwlock );
		p = ((struct process*)htable_retrieve( process_htable, &temp ));
		if ( p != NULL ) pid = p->pid;
	rwlock_release( process_rwlock );

	return pid;
}



// ------------------------------------------------------------


struct process* new_process( const char name[OS_NAME_LENGTH] )

{
	struct process *p = NULL;
	int i;

		p = malloc( sizeof( struct process ) );
		if ( p == NULL ) return NULL;
	
		memcpy( p->name, name, OS_NAME_LENGTH );
	
		p->thread_count = 0;
		p->rc = -1;
		p->state = PROCESS_STOPPED;
		p->flags = 0;
	
		p->lock = init_rwlock();
		if ( p->lock == NULL ) 
		{
			free( p );
			return NULL;
		}
		
		p->kernel_threads = 0;
	
		p->tls_location = NULL;
		
		p->map = new_vmmap();
	

		p->environment = NULL;
		p->environment_count = 0;
		
		p->lfb = NULL;
		p->console = -1;
		p->console_location = NULL;
	

		p->linear_next = NULL;
		p->linear_prev = NULL;
		p->sibling_next = NULL;
		p->sibling_prev = NULL;
		p->parent = NULL;
		p->child = NULL;
	
		p->threads = NULL;
		p->threads_htable = init_htable( 32, 80, 
										 threads_key, 
										 threads_compare );
									 
	
	    p->pid = process_counter++;


		p->usage_seconds 	  = 0;
		p->usage_milliseconds = 0;
	
	
		p->alloc = NULL;
		p->shared = NULL;
		p->waits   = NULL;		// Other threads/processes waiting

   
	
				// Create semaphore area.
		p->sems = (struct semaphore*) malloc( sizeof( struct semaphore ) * LOCAL_SEM_COUNT );
		for ( i = 0; i < LOCAL_SEM_COUNT; i++) 
			p->sems[i].sem_id = -1;

		INLINE_INIT_SPINLOCK( p->sems_lock );

				// Create IPC block.
		for ( i = 0; i < MAX_PORTS; i++ )
		{
			p->ipc[i].tid 			= -1;
			p->ipc[i].flags 		= 0;
			p->ipc[i].pulses 		= NULL;
			p->ipc[i].last_pulses 	= NULL;
			p->ipc[i].messages 		= NULL;
			p->ipc[i].last_message 	= NULL;
		}
	
	
	
		p->data = NULL;
		arch_new_process( p );			// Give the architecture a chance
	
	return p;
};





int delete_process( struct process *p )
{
	release_environment( p );
	clean_shmem( p->pid );
		
	arch_delete_process( p );
	delete_rwlock( p->lock );
	delete_vmmap( p->map );
	delete_htable( p->threads_htable );

	free( p );
	return 0;
}




// ------------------------------------------------------------


int insert_process( int pid, struct process *p )
{
	struct process tmp;
	struct process *temp;
	struct process *parent;

		  tmp.pid = pid;
		  tmp.name[0] = 0;

	rwlock_get_write_access( process_rwlock );
	
		if ( parent >= 0 ) parent = ((struct process*)htable_retrieve( process_htable, &tmp ));
					  else parent = NULL;

		atomic_inc( & process_length );
	
		  p->sibling_next = NULL;
		  p->sibling_prev = NULL;
		  p->linear_next = NULL;
		  p->linear_prev = NULL;
		  p->parent = NULL;
	
	  // hash table insert ---------------------------------------
	  htable_insert( process_htable, p );
	  
		  if ( process_list == NULL )
		  {
			  process_list = p;
			  rwlock_release( process_rwlock );
			  return 0;
		  }

	  
	  // linear insert -------------------------------------------
	  temp = process_list;
	  while (temp->linear_next != NULL) temp = temp->linear_next;
	  temp->linear_next = p;
	  p->linear_prev = temp;

	  // familial insert -----------------------------------------
	  if ( parent != NULL )
	  {
		  if ( parent->child != NULL )  
		  {
			  parent->child->sibling_prev = p;
			  p->sibling_next = parent->child;
		  }
		  else
		  {
			  parent->child = p;
		  }
		  			
		  parent->child = p;
		  p->sibling_prev = NULL;
	  }

	  p->parent = parent;
			  
	 
	rwlock_release( process_rwlock );
	return 0;
}

/// \todo cut and paste the logic from detach_thread to here!!  ..err.. and then modify it until it compiles and works

struct process* detach_process( int pid )
{
	struct process temp;
	struct process *p;

		  temp.pid = pid;
		  temp.name[0] = 0;

	rwlock_get_write_access( process_rwlock );
	
		p = ((struct process*)htable_retrieve( process_htable, &temp ));
		if ( p == NULL )
		{
			rwlock_release( process_rwlock );
			return NULL;
		}


	  // hash table deletion ---------------------------------
	
	  htable_remove( process_htable, p );
	
	  // linear deletion -------------------------------------
	  if ( process_list == p ) process_list = p->linear_next;
	  
	  if ( p->linear_prev != NULL ) 
			  p->linear_prev->linear_next = p->linear_next;
	  if ( p->linear_next != NULL ) 
			  p->linear_next->linear_prev = p->linear_prev;
	  

	  // familial deletion -----------------------------------
	  if ( p->parent != NULL )
	  {
	     if ( p->parent->child == p ) p->parent->child = p->sibling_next;
 	     if ( p->sibling_prev != NULL ) 
				 p->sibling_prev->sibling_next = p->sibling_next;
	     if ( p->sibling_next != NULL ) 
				 p->sibling_next->sibling_prev = p->sibling_prev;
	  }

	  // save the children!!!!  (should we do this first?)


	atomic_dec( & process_length );
	  

	  /// \todo figure out!! COMPLETE THE RE-INSERTION OF THE CHILDREN INTO THE FAMILIAL STRUCTURE
	
	  
	rwlock_release( process_rwlock );
	return p;
}



// ----------------------------------------



// --------------------------------------------------------------



// Acquires a rwlock on the global process list and on the
// process requested, if found. 
//
// flags = READER | WRITER
// 

struct process *checkout_process( int pid,  unsigned int flags )
{
	struct process temp;
	struct process *p;

		  temp.pid = pid;
		  temp.name[0] = 0;

	rwlock_get_read_access( process_rwlock );		// We are reading the process list.

		p = ((struct process*)htable_retrieve( process_htable, &temp ));

		if ( p == NULL )
		{
			// No such PID.
			rwlock_release( process_rwlock );
			return NULL;
		}

	if ( (flags == READER) ) rwlock_get_read_access( p->lock );
						else rwlock_get_write_access( p->lock );
	
	// Leave the global process list locked to ensure data integrity.
	
	return p;
}


// Release a process and the global process list.

void 			commit_process( struct process *p )
{
	rwlock_release( p->lock );
	rwlock_release( process_rwlock );
}





