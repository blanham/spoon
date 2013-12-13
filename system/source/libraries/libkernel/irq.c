#include <kernel.h>



/**  \defgroup IRQ  IRQ Handling  

<P>
IRQ events trigger the kernel IRQ entry code which then
causes the kernel to search for userland IRQ handlers hwich
have been installed during operation.

<P>
If there is some userland IRQ handler associated with an IRQ event,
the kernel code requests an immediate run for the handler via the 
scheduler and then calls the scheduler.

<P>
The kernel IRQ handlers run within the context of the currently 
executing thread but run on the kernel stack. This is for speed.

<P>
A userland application can install it's own IRQ handler for
a specific IRQ event and release it at it's whim. 


  
  
 */


/** @{ */

int smk_request_irq( irq_func fn, int irq_num, const char *name, void *data  )
{
     int ans;	  
     uint32_t d[4];

	d[0] = (uint32_t)fn;
	d[1] = (uint32_t)irq_num;
	d[2] = (uint32_t)data;
	d[3] = 0;

		// spawn a thread for the IRQ handler
       ans = _sysenter( (SYS_THREAD|SYS_ONE) , 
   	              5, 
	              (uint32_t)name, 
		      	  (uint32_t)&d,
		      	  0,
		      	  (uint32_t)__irq_intro);

       if ( ans <= 0 ) return -1;
     
	ans = _sysenter( (SYS_IRQ|SYS_ONE), 0, irq_num, ans, 0, 0 );
	return ans;
}


int smk_release_irq( int irq_num )
{
	return _sysenter( (SYS_IRQ|SYS_TWO), 0, irq_num, 0, 0,0 );
}


int smk_ack_irq( int irq_num, int status )
{
	return _sysenter( (SYS_IRQ|SYS_THREE), irq_num, status, 0,0,0 );
}



void smk_irq_stub( irq_func fn, int irq_num, void *data )
{
	int rc;
	
	while (1==1)
	{
		rc = fn( irq_num, data );
		if ( smk_ack_irq( irq_num, rc ) != 0 ) break;
	}
}


/** @} */
    

