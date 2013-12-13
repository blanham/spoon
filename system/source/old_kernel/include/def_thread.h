#ifndef _KERNEL_DEF_THREAD_H
#define _KERNEL_DEF_THREAD_H


#include <configure.h>
#include <syscalls.h>


struct thread
{
   unsigned char name[OS_NAME_LENGTH];	// thread name
   int tid;				// thread id
   int state;				// thread state
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
   
   struct process *process;

   struct thread *parent;
   struct thread *child;

   struct thread *sibling_next;
   struct thread *sibling_prev;
   struct thread *linear_next;
   struct thread *linear_prev;


   void *data;
   
};


#endif

