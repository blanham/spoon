#ifndef _KERNEL_WAITS_H
#define _KERNEL_WAITS_H

#include <process.h>
#include <threads.h>

struct wait_info 
{
   int pid;						// pid waiting
   int tid;						// tid waiting
   int success;					// indicates successful return of a return code.
   int rc;						// return code
   struct wait_info *next;		// linked list information...
   struct wait_info *prev;		//   ... same.
};

int begin_wait_process( int pid, int *rc );
int begin_wait_thread( int pid, int tid, int *rc );

int clean_process_waits( struct process *proc, int rc );
int clean_thread_waits( struct thread *tr, int rc );

#endif

