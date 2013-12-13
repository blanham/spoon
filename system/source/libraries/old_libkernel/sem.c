#include <types.h>
#include <kernel.h>


/**  \defgroup SEM  Semaphores  
 
There are 1024 global semaphore slots allocated, which means that 1024 semaphores may be
be shared between applications.

<P>
There are also semaphore slots per process which can be acquired and released by any
thread in the process. 

<P>
These are some rules and features I have governing the semaphores at the moment.
<UL>
  <LI>A process may create a local or global semaphore</LI>
  <LI>Only the parent process may delete a semaphore, global or local</LI>
  <LI>Semaphores may have a capacity for 0 or more threads to be holding it at any time.</LI>
  <LI>While a thread is waiting for a semaphore, it is placed into a waiting list and then put to sleep. Only the
      destruction of the semaphore can wake it up</LI>
  <LI>There are semaphore "watchers" which will be put into a sleep mode and only woken up when the
      semaphore that they are watching is released... They will not acquire the semaphore.</LI>
</UL>

<P>
It is also possible to create a semaphore with a capacity of 0.   This is useful because it will 
require an initial release of the semaphore by another process before the waiting process/thread
will be able to acquire it.  It's sort of like a "GO" signal.  This came in handy when I was 
implementing the Looper and looper_thread behaviour.

  
 */

/** @{ */

int smk_acquire_sem(sem_id sem)
{
	return _sysenter( (SYS_SEMAPHORE|SYS_THREE), 0, sem, 0, 0,0);
}

int smk_acquire_sem_etc(sem_id sem, 
		    uint32 count, 
		    uint32 flags, 
		    bigtime_t timeout)
{
	return _sysenter( (SYS_SEMAPHORE|SYS_THREE), 0, sem, 0, 0,0);
}
		  

int smk_create_sem(uint32 thread_count, const char *name)
{
	return _sysenter( (SYS_SEMAPHORE|SYS_ONE), 0, thread_count, 0, 0,0);
}


int smk_delete_sem(sem_id sem)
{
	return _sysenter( (SYS_SEMAPHORE|SYS_TWO), 0, sem, 0, 0,0);
}



int smk_release_sem(sem_id sem)
{
	return _sysenter( (SYS_SEMAPHORE|SYS_FOUR), 0, sem, 0, 0,0);
}


int smk_watch_sem(sem_id sem)
{
	return _sysenter( (SYS_SEMAPHORE|SYS_FIVE), 0, sem, 0, 0,0);
}






int smk_acquire_global_sem(sem_id sem)
{
	return _sysenter( (SYS_SEMAPHORE|SYS_THREE), 1, sem, 0, 0,0);
}

int smk_acquire_global_sem_etc(sem_id sem, 
		    uint32 count, 
		    uint32 flags, 
		    bigtime_t timeout)
{
	return _sysenter( (SYS_SEMAPHORE|SYS_THREE), 1, sem, 0, 0,0);
}
		  

int smk_create_global_sem(uint32 thread_count, const char *name)
{
	return _sysenter( (SYS_SEMAPHORE|SYS_ONE), 1, thread_count, 0, 0,0);
}


int smk_delete_global_sem(sem_id sem)
{
	return _sysenter( (SYS_SEMAPHORE|SYS_TWO), 1, sem, 0, 0,0);
}



int smk_release_global_sem(sem_id sem)
{
	return _sysenter( (SYS_SEMAPHORE|SYS_FOUR), 1, sem, 0, 0,0);
}


int smk_watch_global_sem(sem_id sem)
{
	return _sysenter( (SYS_SEMAPHORE|SYS_FIVE), 1, sem, 0, 0,0);
}


/** @} */

