#include <types.h>
#include <irq.h>
#include <macros.h>
#include <dmesg.h>
#include <atomic.h>
#include <strings.h>
#include <conio.h>
#include <alloc.h>
#include <macros.h>
#include <paging.h>
#include <scheduler.h>

#include <process.h>
#include <threads.h>

static struct irq_handler *handlers;

static int irq_count;
static spinlock irq_spinlock = INIT_SPINLOCK;


void init_irqs()
{
  int i;
  int size;

  dmesg( "setting IRQ handlers.\n" );

  irq_count = arch_init_irqs();
       size = (irq_count * sizeof(struct irq_handler));
   handlers = (struct irq_handler*)malloc( size );
  
  for ( i = 0; i < irq_count; i++)
  {
  	  handlers[i].name[0] = 0;
	  handlers[i].pid     = 0;
	  handlers[i].tid     = 0;
	  handlers[i].masked  = 1;
	  handlers[i].t       = NULL;
  }

}

// *******************************************************



void unmask_irq(int irq)
{
	arch_unmask_irq(irq );
    handlers[ irq ].masked = 0;
}

void mask_irq(int irq)
{
	arch_mask_irq( irq );
    handlers[ irq ].masked = 1;
}

// returns 0 if unmasked

int masked( int irq )
{
    return handlers[ irq ].masked;
}

// ************************************************************************


int request_irq( struct thread *t, char n[OS_NAME_LENGTH], int irq )
{
	int ans;
	
	ans = -1;

	if ( irq < 0 ) return -1;
	if ( irq >= irq_count ) return -1;
	
	acquire_spinlock( &irq_spinlock );
	
	  if ( handlers[irq].pid == 0 )
	  {
		  memcpy( handlers[irq].name, n, OS_NAME_LENGTH );
		  handlers[irq].pid = t->process->pid;
		  handlers[irq].tid = t->tid;
		  handlers[irq].t = t;
		  set_thread_state( t, THREAD_IRQ );
		  ans = 0;
		  if ( masked( irq ) != 0 ) unmask_irq( irq );
		  		// Okay to handle now
	  }

	
	release_spinlock( &irq_spinlock );

	return ans;
}

int release_irq( struct thread *t, int irq )
{
	int ans;
	
	ans = -1;

	if ( irq < 0 ) return -1;
	if ( irq >= irq_count ) return -1;
	
	acquire_spinlock( &irq_spinlock );

	  if ( handlers[irq].pid == t->process->pid )
	   if ( handlers[irq].tid == t->tid )
	   {
		  handlers[irq].pid = 0;
		  handlers[irq].tid = 0;
		  handlers[irq].t = NULL;
		  ans = 0;
		  if ( masked(irq) == 0 ) mask_irq( irq );
	   }
	
	release_spinlock( &irq_spinlock );

	return ans;
}

// ************************************************************************


int irq_handler( int irq_num )
{
  int handled = -1;
 
    // ***** IRQ handling ************************************************
	
  arch_ack_irq( irq_num );
  			// The IRQ will be acknowledged first. 
		
  mask_irq( irq_num );
  			// Then we will mask the irq.

   if (  (handlers[irq_num].pid != 0) && (  handlers[irq_num].tid != 0 ) )
     {
        if ( sched_request( handlers[irq_num].t ) != 0 )
        {	
			/// \todo a graceful death is required here.
		}
  	
		handlers[irq_num].t->state = THREAD_RUNNING;
		handled = IRQ_HANDLED;
     }
   else
    {
		dmesg( "%s%i\n","IRQ: unhandled IRQ event for ",irq_num);
    }



  return handled;
}





