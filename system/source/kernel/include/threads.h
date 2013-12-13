#ifndef _KERNEL_THREADS_H
#define _KERNEL_THREADS_H

#include <limits.h>
#include <gdt.h>


#define sK_BASE			0x80000000ul
#define sK_CEILING		0xFA000000ul
#define sK_PAGES_THREAD	10
#define sK_PAGES_KERNEL	2



#include <ds/family.h>
#include <ds/list.h>
#include <ds/htable.h>

struct process;			// Just declare it.

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




struct thread
{
   unsigned char name[NAME_LENGTH];		// thread name
   int tid;								// thread id
   int state;							// thread state

   int sched_state;						// next requested state

   struct process *process;				// The process
   
/*
   int rc;				// return code.

   int32 priority;

   void *tls;				// thread local storage value


   unsigned int capabilities[ TOTAL_SYSCALLS ];	// syscall permission table
  
   
   int ports[ MAX_PORTS ];					
   int last_port_pulse;
   int last_port_message;

   uint32 sleep_seconds;
   uint32 sleep_milliseconds;
 

   struct wait_info *waits;		// other threads waiting on this
   struct wait_info *active_wait;	// this wait information.
   

   uint32 stack_kernel;
   uint32 stack_user;
   uint32 stack_pages;
*/
   
   int cpu;					// The CPU that this thread is on.
   int cpu_affinity;		// The CPU that this thread prefers.
   
   uint32_t  stack_kernel;
   uint32_t  stack_user;
   uint32_t  stack;

   int math_state;
   uint8_t *math; 			// fpu,mmx,xmm,sse,sse2

   FAMILY_TREE;
   LINKED_LIST;
   
};



int new_thread( int type,
	       	    struct process *proc, 
		   	    struct thread *parent, 
		        const char name[NAME_LENGTH],
			    uint32_t entry,
			    int priority,
			    uint32_t data1,
			    uint32_t data2,
			    uint32_t data3,
			    uint32_t data4 );



struct thread* get_thread( struct process *proc, int tid );

int exec_thread( unsigned int cpu_id, struct thread *t, 
				 unsigned int milliseconds );


int threads_key( void *td );
int threads_compare( void *ad, void *bd );


void set_thread_state( struct thread *t, unsigned int state );


#endif

