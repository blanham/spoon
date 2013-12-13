#include <types.h>
#include <kernel.h>



/**  \defgroup MESSAGES  Message IPC  



<P>
Messages are a way of sending an arbitrary piece of data between two processes or threads
in a system. If you just want to send a block of information, then this IPC method would
be it.

 
 */

/** @{ */

int smk_send_message( int source_port, 
		  int dest_pid,
		  int dest_port,
		  int bytes, void *data )
{
 return _sysenter( SYS_MESSAGE | SYS_ONE,
  		    source_port,
		    dest_pid,
		    dest_port,
		    bytes, 
		    (uint32)data    );
}


int smk_recv_message( int *source_pid,
	          int *source_port,
		  int *dest_port,
		  int bytes, 
		  void *data )
{
  return _sysenter( SYS_MESSAGE | SYS_TWO,
  		    (uint32)source_pid, 
		    (uint32)source_port,
		    (uint32)dest_port,
		    bytes, 
		    (uint32)data );
}

int smk_peek_message( int *source_pid,
	          int *source_port,
		  int *dest_port,
		  int *bytes )
{

  return _sysenter( SYS_MESSAGE | SYS_THREE,
  		    (uint32)source_pid, 
		    (uint32)source_port,
		    (uint32)dest_port,
		    (uint32)bytes, 
		    0 );
}




int smk_drop_message()
{
  return _sysenter( SYS_MESSAGE | SYS_FOUR,
  		    0, 
		    0, 
		    0,
		    0, 
		    0 );
}


/** @} */

