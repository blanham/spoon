#ifndef _KERNEL_SWITCHING_H
#define _KERNEL_SWITCHING_H

#include <threads.h>
#include <process.h>




extern struct process *g_switchingProcess;
extern struct thread  *g_switchingThread;


int init_switching();
int start_switching();


int switch_thread( struct thread * );


/// \todo these macros should be CAPITILIZED ...

#define current_process()	 g_switchingProcess
#define current_thread()	 g_switchingThread

#define current_pid()	(( g_switchingProcess == NULL ) ? -1 : g_switchingProcess->pid)
#define current_tid()	(( g_switchingThread == NULL ) ? -1 : g_switchingThread->tid)


int inside_scheduler();




#endif

