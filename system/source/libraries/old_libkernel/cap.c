#include <types.h>
#include <kernel.h>

/**  \defgroup CAP  Capabilities  

<P>
Each thread has it's own capability array which controls access to the
system calls.  It's basically a bit mask which is AND'd to double-check
that the thread has certain abilities before executing it's request.
<p>
Changes and modifications to the capability list can be achieved through
system calls.
<P>
Capabilities are inherited from the parent thread or parent process.

<P>
Please see the kernel system call list if you want to start limiting the
capabilities of your processes and threads.  
(/spoon/system/source/kernel/SYSCALLS.readme)


  
 */

/** @{ */

int smk_set_remote_capability( int pid, int tid, uint32 set )
{
	return _sysenter( (SYS_CAP|SYS_ONE), 
			  (uint32)pid, 
			  (uint32)tid,
			  set, 0, 0 );
}

/** Do you think this will work?
 *
 */
int	smk_get_remote_capability( int pid, int tid, uint32 call  )
{
	return _sysenter( (SYS_CAP|SYS_TWO), 
			  (uint32)pid, 
			  (uint32)tid,
			  call, 0, 0 );
}


/** And this one?
 */
int smk_set_capability( int tid, uint32 set )
{
	return _sysenter( (SYS_CAP|SYS_THREE), 
			  0,
			  (uint32)tid,
			  set, 0, 0 );
}

/** And this one too?
 */
int	smk_get_capability( int tid, uint32 call )
{
	return _sysenter( (SYS_CAP|SYS_FOUR), 
			  0, 
			  (uint32)tid,
			  call, 0, 0 );
}


/** @} */

