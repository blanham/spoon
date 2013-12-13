
#ifndef _KERNEL_PROCESS_H
#define _KERNEL_PROCESS_H

#include <inttypes.h>
#include <defs.h>

#include <limits.h>

#include <threads.h>

#include <rwlock.h>
#include <stats.h>

#include <ds/list.h>
#include <ds/htable.h>
#include <ds/family.h>


#define PROCESS_INIT			1
#define PROCESS_RUNNING			2
#define PROCESS_STOPPED			3
#define PROCESS_FINISHING		4
#define PROCESS_FINISHED		5
#define PROCESS_CRASHING		6
#define PROCESS_CRASHED			7

extern char *process_states[]; 


struct process
{
   unsigned char name[NAME_LENGTH];		// process name
   unsigned int pid;					// process id
   int state;							// state of the process

   uint32_t *map;						// virtual memory map location

   struct rwlock *lock;					// rwlock for access to process.

   struct thread *threads;	  		 	// Threads belonging to the process.
   struct hash_table *threads_htable;	// Hash table of process' threads.
   int thread_count;					// number of threads in process
   
   int last_tid;						// The last tid used
   

   int kernel_threads;					// # of threads in the kernel.

   unsigned int flags;					// Flag information about the process.
   
   /*

   int rc;								// return code of the process
   void **tls_location;					// location of process tls.
	
   */
 
   void* environment;					// Environment variables. 
   int   environment_count;				// Environment size;
	
   /*
   int console;             			// Console ID used by process
   void* console_location;      		// Console location.

  */

   /*
   uint64_t			st_startTime;		// Start time in milliseconds
   struct s_time	st_runningTime;		// Amount of time on the CPU
   struct s_time	st_syscallTime;		// Amount of time within syscalls
   */
		 

   struct allocated_memory *alloc; 	// Linked list of allocated memory

   /*
   struct ipc_block ipc[MAX_PORTS];	// IPC ports.
   struct shared_memory *shared;   	// Linked list of shared memory
   struct semaphore *sems;	   	// Linked list of processes' semaphores
   spinlock sems_lock;			// Semaphore lock.
   struct wait_info *waits;		// Linked list of waiting pid/tid's
   
   */

   FAMILY_TREE;
   LINKED_LIST;
};



struct process *new_process( const char name[NAME_LENGTH] );
void del_process( struct process *proc );


int insert_process( int pid, struct process *proc );
void remove_process( struct process *proc );


void init_processes();


struct process *checkout_process( int pid,  unsigned int flags );
void 			commit_process( struct process *proc );


#endif


