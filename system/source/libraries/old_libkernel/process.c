#include <types.h>
#include <kernel.h>




/**  \defgroup PROCESS  Process Control  
 
In the context of the spoon microkernel, a process is an abstract
concept to describe a collection of threads which share the same
memory space and process id. (and other stuff, like security permissions, etc)

<P>
In truth, that's about it.

<p>
When a new process is created, a memory space is created and a main thread
is spawned which is given the thread id of 0.  This main thread
may then spawn more threads or function purely on it's own. It's 
the program's choice.

  
 */

/** @{ */

static int32 _libkernel_cached_pid = -1;


int32 smk_find_process_id( const char name[OS_NAME_LENGTH] )
{
	return _sysenter( (SYS_SYSTEM|SYS_ONE), 0, (uint32)name,0,0,0 );
}


int32 smk_find_process_name( int32 pid, const char *name )
{
	return _sysenter( (SYS_SYSTEM|SYS_TWO), pid, (uint32)name, 0,0,0 );
}

int32 smk_process_exists( int32 pid )
{
	return _sysenter( (SYS_SYSTEM|SYS_THREE), pid, 0,0,0,0 );
}




int32 smk_getpid()
{
	int32 a;
	int32 b;

	if ( _libkernel_cached_pid > 0 ) return  _libkernel_cached_pid;
	_sysenter( (SYS_PROCESS|SYS_ONE),(uint32)&a,(uint32)&b,0,0,0 );
	 _libkernel_cached_pid = a;
        return a;
}

int32 smk_start_process( int32 pid )
{
	return _sysenter( (SYS_PROCESS|SYS_TWO),pid,0,0,0,0 );
}

/** @} */

