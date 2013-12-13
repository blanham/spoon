#include <kernel.h>


/**  \defgroup THREADS  Thread Control  
 
<P>
Threads are the basic execution unit in the system.  Anything that is
running is a thread.  Many threads belong to a single process. Each thread
has it's own thread id (tid).
<p>

 
 */

/** @{ */

void __thread_intro();

int32 smk_spawn_thread( thread_func  func, const char *name, int32 priority, void* data )
{
  int32 ans;
  uint32 d[4];

	d[0] = (uint32)func;
	d[1] = (uint32)data;
	d[2] = 0;
	d[3] = 0;

  ans = _sysenter( (SYS_THREAD|SYS_ONE) , 
	         priority, 
		 (uint32)name, 
		 (uint32)&d,
		 0,
		 (uint32)__thread_intro);
  
  return ans;
}


int32 smk_set_thread_state( int32 tid, uint32 state )
{
	int32 ans;
	ans = _sysenter( (SYS_THREAD|SYS_TWO), tid, state,0,0,0 );
	return ans;
}

int32 smk_resume_thread(  int32 tid )
{
  return smk_set_thread_state( tid, THREAD_RUNNING );
}

int32 smk_suspend_thread( int32 tid )
{
  return smk_set_thread_state( tid, THREAD_SUSPENDED );
}

int32 smk_kill_thread( int32 tid )
{
	return smk_set_thread_state( tid, THREAD_FINISHED );	
}


int32 smk_find_thread_id( const char name[OS_NAME_LENGTH] )
{
	return _sysenter( (SYS_THREAD|SYS_THREE), 0, (uint32)name, 0, 0, 0 );
}

int32 smk_find_thread_name( int32 tid, const char *name )
{
	return -1;
}

int32 smk_gettid()
{
	int32 a,b;
	a = 10;
	b = 10;
	_sysenter( (SYS_PROCESS|SYS_ONE),(uint32)&a,(uint32)&b,0,0,0 );
        return b;
}


// --------- TLS support.

int smk_set_tls_location( void* location )
{
   return _sysenter( (SYS_PROCESS|SYS_FOUR),
		     1, 
		     (uint32)location,
		     0,0,0 );
}

int smk_get_tls_location( void** location )
{
   return _sysenter( (SYS_PROCESS|SYS_FOUR),
		     0, 
		     (uint32)location,
		     0,0,0 );
}

int smk_set_tls_value( int tid, void* value )
{
   return _sysenter( (SYS_THREAD|SYS_SIX),
   		     1,
		     tid, 
		     (uint32)value,
		     0,0 );
}

int smk_get_tls_value( int tid, void** value )
{
   return _sysenter( (SYS_THREAD|SYS_SIX),
   		     0,
		     tid,
		     (uint32)value,
		     0,0 );
}


/** @} */

