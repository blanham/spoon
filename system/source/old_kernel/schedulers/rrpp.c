#include <configure.h>
#include <alloc.h>
#include <conio.h>
#include <process.h>
#include <threads.h>
#include <switching.h>
#include <scheduler.h>
#include <interrupts.h>
#include <cpu.h>
#include <macros.h>

#include <smk.h>
#include <smk_idle.h>
#include <smk_gc.h>


/*
 *
 *  schedulers/rrpp.c - round robin per process
 *
 *  This is not SMP safe. 
 *
 *  Nothing is.
 *
 */


struct process_slot
{
	struct process *proc;
	struct thread **threads;
	int length;
	int total;
};


// ******** VARIABLES AND QUEUES ************************


static volatile int	_stop	=	0;	// Used to indicate sched_stop() request
static int			_rc		=	0;	// Used to store value of sched_stop


#define INITIAL_SLOTS			128
#define INITIAL_THREADS			128
#define MAX_REQUESTS			128

static struct process_slot *_queue;				// The queue.
static struct thread* _requests[MAX_REQUESTS];	// The request queue. (IRQ's)

static int _queue_size;
static int _request_pos;
static int _request_mark;



// ************ HELPER FUNCTIONS ************************


// returns 0 if successfully doubled the queue.
int double_queue()
{
	int size;
	int i;
	struct process_slot *big;

	size = (sizeof(struct process_slot) * _queue_size * 2);
	 big = (struct process_slot*)(malloc( size ));
	
	 if ( big == NULL ) return -1;

	 for ( i = 0; i < _queue_size; i++ )
	 {
		big[i] = _queue[i];
	 }

	 for ( i = _queue_size; i < (_queue_size * 2); i++ )
	 {
		big[i].proc 	= NULL;
		big[i].threads 	= NULL;
		big[i].total 	= 0;
	 }

	 free( _queue );

	 _queue = big;
	 _queue_size = _queue_size * 2;
	 
	return 0;
}

// returns 0 if it successfully doubled the slot length
int double_slot( struct process_slot *slot )
{
	int i;
	struct thread **orig;
	struct thread **child;

	orig = slot->threads;

	child = (struct thread**)malloc( slot->total * 2 );
	if ( child == NULL ) return -1;


	for ( i = 0; i < slot->length; i++ )
	{
		child[i] = orig[i];
	}
	
	free( orig );

	slot->threads = child;
	slot->total = slot->total * 2;
	return 0;
}

// ******************************************************


int sched_init()
{
	int i;
	int size;


	_stop = 0;
	  _rc = 0;
	
	_queue_size   = INITIAL_SLOTS;
	_request_pos  = 0;
	_request_mark = 0;

	  // allocate the slots
		  size = (sizeof(struct process_slot) * _queue_size);
		_queue = (struct process_slot*)(malloc( size ));
		
		for ( i = 0; i < _queue_size; i++ ) 
		{
			_queue[i].proc = NULL;
			_queue[i].threads = NULL;
			_queue[i].total = 0;
		}
	
		
	return 0;
}

// Called after scheduler() has returned and in non-tasking 
// environment, we use this function to clean up our structures.
int sched_shutdown()
{
	int i;

		for ( i = 0; i < _queue_size; i++ ) 
		{
			if ( _queue[i].proc != NULL )
				free( _queue[i].threads	);
		}
	
	free( _queue ); 
	return 0;
}



// This is called during normal system execution to request the
// scheduler to stop.  We're going to store the value of _rc
// because it must be returned by scheduler().  We also set a
// variable to indicate to the scheduler() that it must stop.
int sched_stop( int rc )
{
	int enabled = disable_interrupts();
	
		_rc = rc;
		_stop = 1;
	
	if ( enabled == 1 ) enable_interrupts();
	return 0;
}


// ******************************************************

int sched_yield()
{
	return arch_yield();
}

// ******************************************************


int sched_insert( struct thread *t )
{
	int i;
	int pages;
	struct process_slot *slot = NULL;
	int enabled = disable_interrupts();

	  for ( i = 0; i < _queue_size; i++ )
	  {
		if ( _queue[i].proc == NULL ) 
		{
			slot = &(_queue[i]);
			break;
		}

		if ( _queue[i].proc == t->process )
		{
			slot = &(_queue[i]);
			break;
		}
	  }
	  
	 
	  if ( slot == NULL )
	  {
		// Too full and not found. Need to expand _queue 
		slot = &(_queue[_queue_size]); 
		if ( double_queue() != 0 ) 
		{
			if ( enabled == 1 ) enable_interrupts();
			return -1;
		}
	  }
	  
	
	  if ( slot->proc == NULL ) 
	  {
		pages = (sizeof(struct thread*) * INITIAL_THREADS);
		slot->proc 		= t->process;
		slot->threads 	= (struct thread**)(malloc( pages ));
		slot->total 	= pages;
		slot->length 	= 0;
	  }
	  

	  // resize if too small.
	  pages = slot->total / sizeof(struct thread*);
	  if ( slot->length == pages )
	  {
		if ( double_slot(  slot ) != 0 ) 
		{
			if ( enabled == 1 ) enable_interrupts();
			return -1;
		}
	  }
	  
	 // actually add it.
	slot->threads[ slot->length++ ] = t;
		
	if ( enabled == 1 ) enable_interrupts();
	return 0;
}


int sched_remove( struct thread *t )
{
	int i;
	int position;
	int slot_position = 0;
	struct process_slot *slot = NULL;
	int enabled = disable_interrupts();


	for ( i = 0; i < _queue_size; i++ )
	{
		if ( t->process == _queue[i].proc )
		{
			slot = &(_queue[i]);
			slot_position = i;
			break;
		}
	}

	if ( slot == NULL )
	{
		if ( enabled == 1 ) enable_interrupts();
		return -1;
	}
	

	position = -1;
	for ( i = 0; i < slot->length; i++ )
	{
		if ( slot->threads[i] == t )
		{
			position = i;
			break;
		}
	}

	if ( position == - 1 )
	{
		/// \todo die..
		if ( enabled == 1 ) enable_interrupts();
		return -1;
	}
	
	for ( i = position; i < (slot->length - 1); i++ )
	{
		slot->threads[i] = slot->threads[i+1];
	}
	
	if ( --(slot->length) != 0 ) 
	{
		if ( enabled == 1 ) enable_interrupts();
		return 0;
	}
	
	// We need to remove the process as well because it's empty.
	

	free( slot->threads );
	slot->threads = NULL;
	slot->proc = NULL;
	slot->total = 0;


	for ( i = slot_position; i < (_queue_size - 1); i++ )
	{
		_queue[i] = _queue[i+1];
	}

	
	if ( enabled == 1 ) enable_interrupts();
	return 0;
}


int sched_request( struct thread *t )
{
	int enabled = disable_interrupts();

	if ( ((_request_mark+1) % MAX_REQUESTS) == _request_pos )
	{
		if ( enabled == 1 ) enable_interrupts();
		return -1;
	}
	
	_requests[ _request_mark ] = t;
	_request_mark = (_request_mark+1) % MAX_REQUESTS;

	if ( enabled == 1 ) enable_interrupts();
	return 0;
}

									
int sched_ack( struct thread *t )
{
	int enabled = disable_interrupts();

	ASSERT(  _requests[ _request_pos ] == t );
	
	_request_pos = (_request_pos + 1) % MAX_REQUESTS;

	if ( enabled == 1 ) enable_interrupts();
	return 0;
}


// *******  SCHEDULER IMPLEMENTATION ********************



// This is the main scheduler loop which is called by the kernel
// to initiate the running system.
// 
// This particular implementation is extremely slow but it's used
// as an example.


int scheduler()
{
//	int x,y;

		
	int slot_position 		= 0;
	int thread_position 	= 0;
	struct process_slot *slot = NULL;


	int count_perLoop 		= 0;
	
	while ( _stop == 0 )
	{
			
	  // Run the garbage collector if it's required.
		if ( gc_needed() == 0 ) 
		{
			count_perLoop += switch_thread( smk_gc );
		}

	  // Execute a request on every iteration, if there is.
		if ( _request_pos != _request_mark ) 
		{
			count_perLoop += switch_thread( _requests[ _request_pos ] );
		}

		if ( _queue[slot_position].proc == NULL )
		{
			// roll over occured. Run the idle thread if nothing else ran.
			if ( count_perLoop == 0 ) switch_thread( smk_idle ); 
				
			slot_position 	= 0;
			thread_position = 0;
			count_perLoop 	= 0;
		}

		// process sanity check
		slot = &(_queue[slot_position]);
		
		if ( slot->proc == NULL ) continue;	
	
		// thread sanity check
			if ( slot->length <= thread_position )
			{
				slot_position 	+= 1;
				thread_position  = 0;
				slot = &(_queue[slot_position]);
				if ( slot->proc == NULL ) continue;
			}

	
		count_perLoop += switch_thread( slot->threads[ thread_position++ ] );
	}
		
	return _rc;
}




