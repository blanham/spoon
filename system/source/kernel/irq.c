#include <io.h>
#include <atomic.h>
#include <assert.h>
#include <irq.h>
#include <gdt.h>
#include <interrupts.h>
#include <threads.h>
#include <scheduler.h>


#define IRQ_COUNT	16

static unsigned int irq_mask = 0xFFFb;



#define PORT_8259M  0x20
#define PORT_8259S  0xA0


static struct thread* 	handlers[IRQ_COUNT];
static unsigned int 	irq_lock = 0;


/* remap the PIC controller interrupts to our vectors
 * rather than the 8 + 70 as mapped by default 
 *
 * COURTESY OF THE OS FAQ WIKI AT www.mega-tokyo.com/forum
 */

#define ICW1_ICW4       0x01            /* ICW4 (not) needed */
#define ICW1_SINGLE     0x02            /* Single (cascade) mode */
#define ICW1_INTERVAL4  0x04            /* Call address interval 4 (8) */
#define ICW1_LEVEL      0x08            /* Level triggered (edge) mode */
#define ICW1_INIT       0x10            /* Initialization - required! */

#define ICW4_8086       0x01            /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO       0x02            /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE  0x08            /* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0C            /* Buffered mode/master */
#define ICW4_SFNM       0x10            /* Special fully nested (not) */

int remap_irqs( int pic1, int pic2 )
{
	unsigned char a, b;

	a = inb(PORT_8259M);
	b = inb(PORT_8259S);
		
	    outb_p(PORT_8259M, ICW1_INIT + ICW1_ICW4 );
   		outb_p(PORT_8259S, ICW1_INIT + ICW1_ICW4 );

    	outb_p(PORT_8259M+1, pic1 ); 

    	outb_p(PORT_8259S+1, pic2 ); 

    	outb_p(PORT_8259M+1, 0x04);
    	outb_p(PORT_8259S+1, 0x02);

    	outb_p(PORT_8259M+1, ICW4_8086 );
    	outb_p(PORT_8259S+1, ICW4_8086 );


	outb_p(PORT_8259M+1, a);
	outb_p(PORT_8259S+1, b);

	return 0;
}


static inline int masked(int irq)
{
	return (irq_mask & ((unsigned int)( 1 << irq )));
}



static inline void unmask_irq(int irq)
{
  irq_mask = irq_mask &  ~((unsigned int)( 1 << irq ));

  if ( irq < 8 ) outb_p( PORT_8259M + 1, (irq_mask & 0xFF) );
		    else outb_p( PORT_8259S + 1, (( irq_mask >> 8 ) & 0xFF) );
}

static inline void mask_irq(int irq)
{
  irq_mask = irq_mask | ((unsigned int)( 1 << irq ));

  if ( irq < 8 ) outb_p( PORT_8259M + 1, (irq_mask & 0xFF) );
   			else outb_p( PORT_8259S + 1, ((irq_mask >> 8) & 0xFF) );
}


static inline void ack_irq( int irq )
{
  if ( irq > 7 )  outb_p(0xA0,0x20);
  outb_p(0x20,0x20); 
}



void init_irqs()
{
	int i;
	assert( irq_lock == 0 );
	
	remap_irqs( 0x20, 0x28 );

	outb_p( PORT_8259M + 1, irq_mask & 0xFF );
	outb_p( PORT_8259S + 1, ( irq_mask >> 8 ) & 0xFF );

	for ( i = 0; i < IRQ_COUNT; i++ )
		handlers[i] = NULL;
}


void load_irqs( unsigned int cpu_id )
{
	#define CREATE( id, num, function )						\
				extern void function();						\
	  			set_interrupt( id, num,						\
						(IDT_PRESENT | IDT_32 | IDT_INT), 	\
						GDT_SYSTEMCODE,						\
						(uint32_t)function,					\
						IDT_DPL0	);
		
		CREATE( cpu_id, 0x21, __irq_handler1 );
		CREATE( cpu_id, 0x22, __irq_handler2 );
		CREATE( cpu_id, 0x23, __irq_handler3 );
		CREATE( cpu_id, 0x24, __irq_handler4 );
		CREATE( cpu_id, 0x25, __irq_handler5 );
		CREATE( cpu_id, 0x26, __irq_handler6 );
		CREATE( cpu_id, 0x27, __irq_handler7 );
		CREATE( cpu_id, 0x28, __irq_handler8 );
		CREATE( cpu_id, 0x29, __irq_handler9 );
		CREATE( cpu_id, 0x2A, __irq_handlerA );
		CREATE( cpu_id, 0x2B, __irq_handlerB );
		CREATE( cpu_id, 0x2C, __irq_handlerC );
		CREATE( cpu_id, 0x2D, __irq_handlerD );
		CREATE( cpu_id, 0x2E, __irq_handlerE );
		CREATE( cpu_id, 0x2F, __irq_handlerF );
}



int request_irq( struct thread *t, int irq )
{
	int ans = -1;
	
	if ( (irq < 0) || (irq >= IRQ_COUNT) ) return -1;
	
	acquire_spinlock( &irq_lock );
	
	  if ( handlers[irq] == NULL )
	  {
		  handlers[irq] = t;
		  
		  t->state = THREAD_IRQ;

		  if ( masked( irq ) != 0 ) unmask_irq( irq );
		  ans = 0;
	  }

	
	release_spinlock( &irq_lock );
	return ans;
}

int release_irq( struct thread *t, int irq )
{
	int ans = -1;

	if ( (irq < 0) || (irq >= IRQ_COUNT) ) return -1;
	
	acquire_spinlock( &irq_lock );

	  if ( handlers[irq] == t )
	  {
		  handlers[irq] = NULL;
		  if ( masked(irq) == 0 ) mask_irq( irq );
		  ans = 0;
	  }
	
	release_spinlock( &irq_lock );
	return ans;
}



int irq_ack( struct thread *t, int irq, int status )
{
	acquire_spinlock( &irq_lock );

	dmesg("%!ACK with status %i for %i\n", status, irq );

		if ( status == 0 )
		{
			unmask_irq( irq );
			release_spinlock( &irq_lock );
			return 0;
		}

	dmesg("%!Unhandled IRQ %i\n", irq );
	release_spinlock( &irq_lock );
	return 0;
}



int irq_handler( int irq )
{
	acquire_spinlock( &irq_lock );
	mask_irq( irq );

	dmesg("%!IRQ %i occurred on %i\n", irq, CPUID );
	
		if ( handlers[irq] != NULL )
		{
			queue( handlers[irq] );
		}
	

	ack_irq( irq );
	release_spinlock( &irq_lock );
	return 0;
}





