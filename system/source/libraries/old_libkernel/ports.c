#include <types.h>
#include <kernel.h>




/**  \defgroup PORTS  IPC Port Control  
 

<P>
The concept of ports in the microkernel is similar to ports in TCP.  Each process has
1024 ports which can be handled by threads.  Whenever an IPC communication needs to
be sent, a destination process and a destination port must be specified.
<P>
All ports are closed by default and any IPC to a closed port is immediately rejected. The
port must be opened and the port handler ( a thread, any thread ) needs to be valid.



  
 */

/** @{ */

//  5 Jan 2005 - modifications by Christopher Nelson to include
//               the userland portmap and atomic un/setting.
  

// -------------------------------------------------------
//static char   _libkernel_portmap[1024];
//static uint32 _libkernel_port_lock = 0;
//static uint8  _libkernel_portmap_init = 0;
// -------------------------------------------------------



int smk_create_port( int port, int tid )
{
  int ret;
  if ( (port < 0) || (port > 1024) ) return -1;
  ret = _sysenter( (SYS_PORT|SYS_ONE), port, tid, 0, 0, 0 );
  return ret;
}


int smk_port2tid( int port )
{
  if ( (port < 0) || (port > 1024) ) return -1;
  return _sysenter( (SYS_PORT|SYS_TWO), port, 0, 0, 0, 0 );
}

int smk_tid2port( int tid )
{
  return _sysenter( (SYS_PORT|SYS_THREE), tid, 0, 0, 0, 0 );
}

int smk_release_port( int port )
{
  if ( (port < 0) || (port > 1024) ) return -1;
  return _sysenter( (SYS_PORT|SYS_FOUR), port, 0, 0, 0, 0 );
}


int smk_mask_port( int port )
{
  if ( (port < -1) || (port > 1024) ) return -1;
  return _sysenter( (SYS_PORT|SYS_FIVE), port, 1, 0, 0, 0 );
}

int smk_unmask_port( int port )
{
  if ( (port < -1) || (port > 1024) ) return -1;
  return _sysenter( (SYS_PORT|SYS_FIVE), port,  0, 0, 0, 0 );
}


int smk_save_port_masks( struct port_pair masks[1024] )
{
  return _sysenter( (SYS_PORT|SYS_SIX), 0, (uint32)&(masks[0]),  0, 0, 0 );
}

int smk_restore_port_masks( struct port_pair masks[1024] )
{
  return _sysenter( (SYS_PORT|SYS_SIX), 1, (uint32)&(masks[0]),  0, 0, 0 );
}



// -------------------------------------------------------
int smk_new_port( int tid )
{
  int port = 0;

    while (  smk_create_port( port, tid ) != 0 ) 
       port = (port+1) % 1024;

   return port;
}

// -------------------------------------------------------



/*
int smk_alloc_port()
{
  int port = -1;
  uint32 index;
  
   smk_acquire_spinlock( & _libkernel_port_lock );
   
   if (_libkernel_portmap_init==0)
   {
	kmemset(_libkernel_portmap, 0xff, 32 * 4 );
	_libkernel_portmap_init=1;   
   }
   
   // Note that this is backwards! A set bit indicates that the port is
   //   FREE, a cleared bit indicates that it's ALLOCATED! 
   for ( index = 0; index < 32; index++ )
   {
     uint32 bit;
     if (_libkernel_portmap[index]==0) continue;
    
     asm volatile("bsfl %1, %0" 
                   : "=r" (bit) 
		   : "rm" (_libkernel_portmap) );
         
     port = (index<<5) + (bit);
     break;
   }
   
   // Allocate the port by marking it as allocated, even 
   // though we haven't created a port for it yet.
   if (port != -1)
   {
     asm volatile(" lock; btrl %1, %0" 
                  : "=m" (_libkernel_portmap) 
		  : "r" (port) );
   }
  
   smk_release_spinlock( & _libkernel_port_lock );
   return port;
}
*/



/** @} */
