#ifndef _KERNEL_THREAD_H
#define _KERNEL_THREAD_H 


#include <types.h>
#include <lengths.h>


#define USER_THREAD			0
#define SYSTEM_THREAD		1


#define THREAD_PULSE_QUEUE		(100 * 6)

//------------

#define THREAD_SYSTEM			0
#define THREAD_RUNNING			1
#define THREAD_SUSPENDED		2
#define THREAD_DORMANT			3
#define THREAD_STOPPED			4
#define THREAD_SEMAPHORE		5
#define THREAD_SLEEPING			6
#define THREAD_WAITING			7
#define THREAD_FINISHED			8
#define THREAD_CRASHED			9
#define THREAD_IRQ				10

extern	int	thread_length;


extern char *states[];




#include "def_thread.h"


int new_thread( int type,
	           struct process *proc, 
		       struct thread *parent, 
		       const char name[OS_NAME_LENGTH],
			   uint32 entry,
			   uint32 priority,
			   uint32 data1,
			   uint32 data2,
			   uint32 data3,
			   uint32 data4 );

int delete_thread( struct thread *t );


void attach_thread( struct thread *parent, struct thread *t );
void detach_thread( struct thread *t );

		           
void set_thread_state( struct thread *t, uint32 state );



struct thread* find_thread_with_id( struct process *p, int id );
struct thread* find_thread_with_name( struct process *p, const char name[OS_NAME_LENGTH] );

int go_dormant( int milliseconds );

// hash table functions.
int threads_key( void * );
int threads_compare( void *, void * );



#endif


