#ifndef _KERNEL_DEF_PROCESS_H
#define _KERNEL_DEF_PROCESS_H

#include <types.h>
#include <lengths.h>



#define		IOPRIV		(1<<0)

struct process
{
   unsigned char name[OS_NAME_LENGTH];	// process name
   int pid;								// process id
   unsigned int state;					// state of the process
   int rc;								// return code of the process

   struct rwlock *lock;					// rwlock for access to process.

   int kernel_threads;					// The number of threads inside the kernel.

   unsigned int flags;
   
   vmmap *map;							// virtual memory map location

   void **tls_location;					// location of process tls.
	
   struct thread *threads;	  		 	// Linked list of process' threads.
   struct hash_table *threads_htable;	// Hash table of process' threads.
   int thread_count;					// number of threads in process
 
   void* environment;					// Environment variables. 
   int   environment_count;				// Environment size;

   void* lfb;
	
   int console;             			// Console ID used by process
   void* console_location;      		// Console location.

		 
   time_t	usage_seconds;
   time_t	usage_milliseconds;
   


   struct allocated_memory *alloc; 	// Linked list of allocated memory


   struct ipc_block ipc[MAX_PORTS];	// IPC ports.
   struct shared_memory *shared;   	// Linked list of shared memory
   struct semaphore *sems;	   	// Linked list of processes' semaphores
   spinlock sems_lock;			// Semaphore lock.
   struct wait_info *waits;		// Linked list of waiting pid/tid's
   
   
   struct process *parent;
   struct process *child;
   struct process *sibling_prev;
   struct process *sibling_next;
   struct process *linear_prev;
   struct process *linear_next;


   void *data;
};


#endif



