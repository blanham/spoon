#ifndef _KERNEL_SCHEDULER_H
#define _KERNEL_SCHEDULER_H

#include "threads.h"

/*
 *  These are the scheduler declarations used by the rest of the kernel
 *  for scheduler manipulation. 
 *
 *  If you're going to write your own scheduler, you need to make sure
 *  that you provide the following functions.
 *
 *
 *  Please note that the following must be true about your implementations:
 * 
 *
 * 		1. They must be atomic and thread-safe	- can be called by multiple
 * 		   threads without causing damage.
 *
 * 		2. They are non-interruptable - provide cli/sti wrappers around any
 * 		   of your code.
 * 
 *
 * 		3. All functions (exception scheduler()) return 0 on success or
 * 		   -1 on failure.
 *
 * 		4. scheduler() will return the value which was given in sched_stop()
 * 
 * 		5. Requested threads will not block any other threads as
 * 		   they may be waiting on resources from others.
 *
 *		6. Your implementations may be called from within scheduler() and not
 *		   cause any problems.
 *
 *  Please not that the following is true about the system:
 * 
 *
 * 		1. The system guarantees that the memory functions will be
 *  	   available for use in your scheduler routines except:
 *
 *  	   		scheduler()
 *
 * 		2. The thread/context from which the scheduler/switch_thread
 * 		   returns may no longer be valid. 
 * 
 *
 * 	For a nice, simple example, please see the round robin scheduler in 
 * 	schedulers/rr.c
 * 
 * 
 */


// scheduler general


int sched_init();		// use to set up your structures
int sched_shutdown();	// use to clean up your structures

int sched_stop( int rc );	// triggers the scheduler() to return/stop on next run
							// The value passed to it must be returned by
							// the scheduler() loop.


int sched_yield();			// release the rest of the timeslice;
int arch_yield();			// architecture yield call.

// scheduler queue manipulation 

int sched_insert( struct thread * );  // queues a thread to run
int sched_remove( struct thread * );  // dequeues a thread from the run queue
int sched_request( struct thread * ); // used to request an immediate run
									  // to handle important stuff, like IRQ's.

int sched_ack( struct thread * ); // Used to remove/cancel a previously
								  // requested thread.


// scheduler implementation

int scheduler();	// The main scheduler implementation.

					// This is a continuous loop which will keep running until
					// another part of the kernel calls sched_stop()
					//
					// It will return the value passed to it by sched_stop();




#endif


