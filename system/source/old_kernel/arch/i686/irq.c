#include <conio.h>
#include <macros.h>
#include <dmesg.h>
#include <memory.h>
#include <paging.h>
#include <scheduler.h>
#include <irq.h>

#include <interrupts.h>

#include "io.h"
#include "irq.h"
#include "gdt.h"
#include "int.h"
#include "eflags.h"
#include "tss.h"
#include "time.h"

//
int irq_handler( int irq_num );

// ----------- external declarations. Look inside jump.S ---
 void __irq_handler1();
 void __irq_handler2();
 void __irq_handler3();
 void __irq_handler4();
 void __irq_handler5();
 void __irq_handler6();
 void __irq_handler7();
 void __irq_handler8();
 void __irq_handler9();
 void __irq_handlerA();
 void __irq_handlerB();
 void __irq_handlerC();
 void __irq_handlerD();
 void __irq_handlerE();
 void __irq_handlerF();

// ----------------------------------------------------

int create_irq_task( uint32 int_num,  uint32 code );

int arch_mask_irq( int irq );
int arch_unmask_irq( int irq );
int arch_ack_irq( int irq );

int remap_irqs( int pic1, int pic2 );


                     // PIC port numbers
#define PORT_8259M  0x20
#define PORT_8259S  0xA0

#define IRQ_COUNT	16

static uint16 irq_mask = 0xFFFF;

int arch_init_irqs()
{
  irq_mask = 0xFFFF;
		
      create_irq_task( 0x21, (uint32) __irq_handler1 );
      create_irq_task( 0x22, (uint32) __irq_handler2 );
      create_irq_task( 0x23, (uint32) __irq_handler3 );
      create_irq_task( 0x24, (uint32) __irq_handler4 );
      create_irq_task( 0x25, (uint32) __irq_handler5 );
      create_irq_task( 0x26, (uint32) __irq_handler6 );
      create_irq_task( 0x27, (uint32) __irq_handler7 );
      create_irq_task( 0x28, (uint32) __irq_handler8 );
      create_irq_task( 0x29, (uint32) __irq_handler9 );
      create_irq_task( 0x2A, (uint32) __irq_handlerA );
      create_irq_task( 0x2B, (uint32) __irq_handlerB );
      create_irq_task( 0x2C, (uint32) __irq_handlerC );
      create_irq_task( 0x2D, (uint32) __irq_handlerD );
      create_irq_task( 0x2E, (uint32) __irq_handlerE );
      create_irq_task( 0x2F, (uint32) __irq_handlerF );
  
 
  dmesg("remapping and masking all IRQ's\n"); 


  remap_irqs( 0x20, 0x28 );

  outb_p( PORT_8259M + 1, irq_mask & 0xFF );
  					// Mask all lower IRQ's
  outb_p( PORT_8259S + 1, ( irq_mask >> 8 ) & 0xFF );
  					// Mask all higher IRQ's

  return IRQ_COUNT;
}

// *******************************************************


int create_irq_task( uint32 int_num, uint32 code )
{
  set_interrupt( int_num,  IDT_PRESENT | IDT_32 | IDT_INT, 
				  			system_CS() , code, IDT_DPL0 );
  return 0;
}

// *******************************************************



// COURTESY OF THE OS FAQ WIKI AT www.mega-tokyo.com/forum

/* remap the PIC controller interrupts to our vectors
 *    rather than the 8 + 70 as mapped by default */

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



int arch_unmask_irq(int irq)
{
  static int here = 0;

  irq_mask = irq_mask - ( 1 << irq );

  if ( irq < 8 ) outb_p( PORT_8259M + 1, (irq_mask & 0xFF) );
  else 
	{
		outb_p( PORT_8259S + 1, (( irq_mask >> 8 ) & 0xFF) );

	    if ( here == 0 ) 
		{
		   here = 1;
		   unmask_irq( 2 );
	    }

	}

  return 0;
}

int arch_mask_irq(int irq)
{
  irq_mask = irq_mask + ( 1 << irq );

  if ( irq < 8 ) outb_p( PORT_8259M + 1, (irq_mask & 0xFF) );
   			else outb_p( PORT_8259S + 1, ((irq_mask >> 8) & 0xFF) );

  return 0;
}


int arch_ack_irq( int irq )
{
  if ( irq > 7 )  outb_p(0xA0,0x20);

  outb_p(0x20,0x20); 
  return 0;
}

// ************************************************************************


int arch_irq_handler( int irq )
{
	int ans = irq_handler(irq);

	if ( ans == IRQ_HANDLED ) sched_yield();
   
	return ans;
}





