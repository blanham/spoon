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

int smk_spawn_thread( thread_func  func, const char *name, int priority, void* data )
{
  int ans;
  uint32_t d[4];

	d[0] = (uint32_t)func;
	d[1] = (uint32_t)data;
	d[2] = 0;
	d[3] = 0;

  ans = _sysenter( (SYS_THREAD|SYS_ONE) , 
	         priority, 
			 (uint32_t)name, 
			 (uint32_t)&d,
			 0,
			 (uint32_t)__thread_intro);
  
  return ans;
}


int smk_set_thread_state( int tid, unsigned int state )
{
	int ans;
	ans = _sysenter( (SYS_THREAD|SYS_TWO), tid, state,0,0,0 );
	return ans;
}

int smk_resume_thread(  int tid )
{
  return smk_set_thread_state( tid, THREAD_RUNNING );
}

int smk_suspend_thread( int tid )
{
  return smk_set_thread_state( tid, THREAD_SUSPENDED );
}

int smk_kill_thread( int tid )
{
	return smk_set_thread_state( tid, THREAD_FINISHED );	
}


/*
int smk_find_thread_id( const char *name )
{
	return _sysenter( (SYS_THREAD|SYS_THREE), 0, (uint32_t)name, 0, 0, 0 );
}
*/


int smk_gettid()
{
	int a,b;
	a = 10;
	b = 10;
	_sysenter( (SYS_PROCESS|SYS_ONE),(uint32_t)&a,(uint32_t)&b,0,0,0 );
    return b;
}


// --------- TLS support.

/*
int smk_set_tls_location( void* location )
{
   return _sysenter( (SYS_PROCESS|SYS_FOUR),
		     1, 
		     (uint32_t)location,
		     0,0,0 );
}

int smk_get_tls_location( void** location )
{
   return _sysenter( (SYS_PROCESS|SYS_FOUR),
		     0, 
		     (uint32_t)location,
		     0,0,0 );
}

int smk_set_tls_value( int tid, void* value )
{
   return _sysenter( (SYS_THREAD|SYS_SIX),
   		     1,
		     tid, 
		     (uint32_t)value,
		     0,0 );
}

int smk_get_tls_value( int tid, void** value )
{
   return _sysenter( (SYS_THREAD|SYS_SIX),
   		     0,
		     tid,
		     (uint32_t)value,
		     0,0 );
}
*/


/** @} */

