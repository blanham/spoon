#include <atomic.h>
#include <inttypes.h>
#include <interrupts.h>
#include <cpu.h>
#include <assert.h>

#include <eflags.h>
#include <misc.h>

#include <alloc.h>


#include <smk.h>
#include <threads.h>

#include <apic.h>

#include <rwlock.h>

#include <stats.h>

#define INITIAL_SCHED_SIZE		1024




#define TIMESLICE		10
							// in milliseconds



void load_scheduler( unsigned int cpu_id )
{
	struct scheduler_info *si = &(cpu[ cpu_id ].sched);

	int i = sizeof( struct schedule ) * INITIAL_SCHED_SIZE;
		
	si->sched_queue = (struct schedule*)malloc( i );
	si->pause_queue = (struct schedule*)malloc( i );

	for ( i = 0; i < INITIAL_SCHED_SIZE; i++ )
	{
		si->sched_queue[i].tr = NULL;
		si->pause_queue[i].tr = NULL;
	}


	si->cpu_id = cpu_id;

	si->current_thread = NULL;
	
	si->sched_count = 0;
	si->pause_count = 0;
	si->sched_size = INITIAL_SCHED_SIZE;
	si->pause_size = INITIAL_SCHED_SIZE;

	si->lock_scheduler = 0;
	si->locked = 0;
	si->running = 0;

	si->position = 0;
}





static int sub_queue( struct scheduler_info *si, struct thread *tr )
{
	int selfMutilation = 0;


	// Are we queueing onto our own CPU?
	if ( CPUID == si->cpu_id )
	{
		// Take note and stop us from being scheduled out.
		selfMutilation = 1;
		si->locked = 1;
	}

	acquire_spinlock( &(si->lock_scheduler) );

	// Because we're adding to the list, we don't need to ensure that
	// the scheduler isn't running. We can just work.


		if ( si->sched_count == si->sched_size ) 
		{
			// too damn full, damn it.	
			release_spinlock( &(si->lock_scheduler) );
			if ( selfMutilation == 1 ) si->locked = 0; // Allowed to continue
			return -1;
		}


		// Add it.
		
		si->sched_queue[ si->sched_count ].tr 	    = tr;
		si->sched_queue[ si->sched_count ].time 	= 0;
		
		si->sched_count += 1;

	
		
	release_spinlock( &(si->lock_scheduler) );

	if ( selfMutilation == 1 ) si->locked = 0; // Allowed to continue
	
	return 0;
}


static int sub_dequeue( struct scheduler_info *si, struct thread *tr )
{
	int pos;
	int i;
	int selfMutilation = 0;
	
	// Are we dequeueing from our own CPU?
	if ( CPUID == si->cpu_id )
	{
		// Take note and stop any more scheduling on this CPU.
		selfMutilation = 1;
		si->locked = 1;
	}

	acquire_spinlock( &(si->lock_scheduler) );

	/// \todo Evaluate the need to wait IFF not the current thread.
	
	// If it's another CPU, make sure it's not running anything.
	if ( selfMutilation == 0 )
		while ( si->running == 1 ) continue;	// thread finished.

	

		// Find it's position.
		for ( pos = 0; pos < si->sched_count; pos++ )
			if ( si->sched_queue[ pos ].tr == tr ) break;

		assert( pos != si->sched_count );	// It was actually there.

		// Close the gap. (I know it's not ideal)
		for ( i = pos; i < (si->sched_count - 1); i++ )
			si->sched_queue[ i ] = si->sched_queue[ i + 1 ];

		// Clean & keep track
		si->sched_count -= 1;

		if ( si->sched_count == 0 ) si->position = 0;
		else
			si->position = ((si->position-1) % si->sched_count);
							// safe because position is unsigned


	release_spinlock( &(si->lock_scheduler) );

	if ( selfMutilation == 1 ) si->locked = 0; // Allowed to continue
	
	return 0;
}

static int sub_pause( struct scheduler_info *si, struct thread *tr, 
						uint64_t milliseconds )
{
	int pos;
	int i;
	int selfMutilation = 0;

	uint64_t target = milliseconds;
	
	// Are we dequeueing from our own CPU?
	if ( CPUID == si->cpu_id )
	{
		// Take note and stop any more scheduling on this CPU.
		selfMutilation = 1;
		si->locked = 1;
	}

	acquire_spinlock( &(si->lock_scheduler) );

	/// \todo Evaluate the need to wait IFF not the current thread.
	
	// If it's another CPU, make sure it's not running anything.
	if ( selfMutilation == 0 )
		while ( si->running == 1 ) continue;	// thread finished.

	
	// QUEUE REMOVAL ........................

		// Find it's position.
		for ( pos = 0; pos < si->sched_count; pos++ )
			if ( si->sched_queue[ pos ].tr == tr ) break;

		assert( pos != si->sched_count );	// It was actually there.

		// Close the gap. (I know it's not ideal)
		for ( i = pos; i < (si->sched_count - 1); i++ )
			si->sched_queue[ i ] = si->sched_queue[ i + 1 ];

		// Clean & keep track
		si->sched_queue[ si->sched_count ].tr = NULL;
		si->sched_count -= 1;

		if ( si->sched_count == 0 ) si->position = 0;
		else
			si->position = ((si->position-1) % si->sched_count);
							// safe because position is unsigned

	// PAUSE INSERTION ......................
		
		if ( si->pause_count == si->pause_size ) 
		{
			// too damn full, damn it.	
			release_spinlock( &(si->lock_scheduler) );
			if ( selfMutilation == 1 ) si->locked = 0; // Allowed to continue
			return -1;
		}


		// Add it by finding the position in the queue.
		
		for ( pos = 0; pos < si->pause_count; pos++ )
		{
			if ( si->pause_queue[ pos ].time > target ) break; 
		}
		
		// Move everything up one.
		for ( i = si->pause_count; i > pos; i-- )
		{
			si->pause_queue[ i ] = si->pause_queue[ i - 1 ];
		}

		// Insert the pause
		
		si->pause_queue[ pos ].tr 	    = tr;
		si->pause_queue[ pos ].time 	= target;
		
		si->pause_count += 1;


	release_spinlock( &(si->lock_scheduler) );

	if ( selfMutilation == 1 ) si->locked = 0; // Allowed to continue
	
	return 0;
}

static int sub_restart( struct scheduler_info *si, struct thread *tr )
{
	int pos;
	int i;
	int selfMutilation = 0;

	// Are we dequeueing from our own CPU?
	if ( CPUID == si->cpu_id )
	{
		// Take note and stop any more scheduling on this CPU.
		selfMutilation = 1;
		si->locked = 1;
	}

	acquire_spinlock( &(si->lock_scheduler) );

	/// \todo Evaluate the need to wait IFF not the current thread.
	
	// If it's another CPU, make sure it's not running anything.
	if ( selfMutilation == 0 )
		while ( si->running == 1 ) continue;	// thread finished.

	
	// PAUSE REMOVAL ........................

		// Find it's position.
		for ( pos = 0; pos < si->pause_count; pos++ )
			if ( si->pause_queue[ pos ].tr == tr ) break;

		assert( pos != si->pause_count );	// It was actually there.

		// Close the gap. (I know it's not ideal)
		for ( i = pos; i < (si->pause_count - 1); i++ )
			si->pause_queue[ i ] = si->pause_queue[ i + 1 ];

		// Keep track
		si->pause_count -= 1;

	// QUEUE INSERTION ......................
		
		if ( si->sched_count == si->sched_size ) 
		{
			// too damn full, damn it.	
			release_spinlock( &(si->lock_scheduler) );
			if ( selfMutilation == 1 ) si->locked = 0; // Allowed to continue
			return -1;
		}


		// Add it.
		
		si->sched_queue[ si->sched_count ].tr 	    = tr;
		si->sched_queue[ si->sched_count ].time 	= 0;
		
		si->sched_count += 1;


	release_spinlock( &(si->lock_scheduler) );

	if ( selfMutilation == 1 ) si->locked = 0; // Allowed to continue
	
	return 0;

}


// ****

/// \todo Improve allocation techniques..

int queue( struct thread *tr )
{
	int id = ( tr->cpu_affinity >= 0 ) ? 
					tr->cpu_affinity : ((tr->tid - 1) % cpu_count);

	assert( tr != NULL );
	assert( tr->cpu < 0 );

	tr->cpu = id;

	dmesg("%!Queued %i.%i onto %i\n", tr->process->pid, tr->tid, id );
	
	return sub_queue( &( cpu[ id ].sched ), tr );
}

int dequeue( struct thread *tr )
{
	int ans;
	int id = tr->cpu;

	assert( tr != NULL );
	assert( tr->cpu >= 0 );
	
	ans = sub_dequeue( &( cpu[ id ].sched ), tr );
	tr->cpu = -1;

	dmesg("%!Dequeued %i.%i onto %i\n", tr->process->pid, tr->tid, id );
	
	return ans;
}

int pause( struct thread *tr, uint64_t milliseconds )
{
	int id = tr->cpu;
	uint64_t distance = milliseconds;
	assert( tr != NULL );

	distance += cpu[ id ].st_systemTime.usage;
	return sub_pause( &( cpu[ id ].sched ), tr, distance );
}

int restart( struct thread *tr )
{
	int id = tr->cpu;
	assert( tr != NULL );
	return sub_restart( &( cpu[ id ].sched ), tr );
}



void remove_last( struct scheduler_info *si, 
				  unsigned int *previous_pos,
				  struct thread *tr
				  )
{
	int i;
	// Honour the thread's requested state change
	
	if ( (si->sched_count != 0)  && (previous_pos >= 0))
	{
		*previous_pos = *previous_pos % si->sched_count;
		if ( *previous_pos < si->sched_count )
			if ( si->sched_queue[ *previous_pos ].tr == tr )
			{
				if ( tr->sched_state >= 0 )
				{
					assert( tr->sched_state != THREAD_RUNNING );
						
					// The last thread wants a state change out of here.
					tr->state = tr->sched_state;
					tr->sched_state = -1;
					tr->cpu = -1;

					// Close the gap. (I know it's not ideal)
					for ( i = *previous_pos; i < (si->sched_count - 1); i++ )
						si->sched_queue[ i ] = si->sched_queue[ i + 1 ];
			
					// Keep track
					si->sched_count -= 1;
			
					// Correct position. 
					if ( si->sched_count == 0 ) si->position = 0;
					else
						si->position = ((si->position-1) % si->sched_count);
				}
		}
	}
}


void scheduler()
{
	unsigned int cpu_id = CPUID; 
	struct scheduler_info *si = &(cpu[ cpu_id ].sched);
	struct thread *tr;
	unsigned int previous_pos;

	// We will never go back.
	set_cpu_flags(  cpu_flags() & ~EFLAG_NESTED_TASK );

	// Now we're working.
	acquire_spinlock( &(si->lock_scheduler) );

	// Start timing the scheduler
	stats_time_start( &(cpu[ cpu_id ].st_schedulerTime) );

	
	while (1==1)
	{
		// If there's nothing to do, do idle.
		if ( si->sched_count == 0 ) 
		{
			previous_pos = -1;
			tr = smk_idle[ cpu_id ];
		}
		else
		{
			previous_pos = si->position;

			tr = si->sched_queue[ si->position ].tr;
			si->position = (si->position + 1) % si->sched_count;
		}


		// And run the selected thread.
		exec_thread( cpu_id, tr, TIMESLICE );
		
		stats_time( cpu_id, &(cpu[ cpu_id ].st_systemTime) ); // Maintain CPU time.


		remove_last( si, &previous_pos, tr );
	
		((char*)0xB8000)[800 + cpu_id * 2] ++ ;
	}

}


