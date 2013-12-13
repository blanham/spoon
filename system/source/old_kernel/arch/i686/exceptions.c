#include <types.h>
#include <macros.h>
#include <dmesg.h>
#include <configure.h>
#include <memory.h>
#include <paging.h>
#include <threads.h>
#include <ioperms.h>
#include <process.h>
#include <switching.h>

#include "io.h"
#include "gdt.h"
#include "eflags.h"
#include "int.h"
#include "misc.h"
#include "tss.h"
#include "fpu.h"
#include "paging.h"
#include "exceptions.h"
#include "thread_data.h"



static char *exceptions[] = 
{
    "Divide-by-zero",			//	0
    "Debug Exception",			//	1
    "NMI",				//	2
    "Breakpoint",			//	3
    "INTO",				//	4
    "BOUNDS",				//	5
    "Invalid Opcode",			//	6
    "Device Not Available",		//	7
    "Double-fault",			//	8
    "Coprocessor segment overrun",	//	9
    "Invalid TSS fault",		//	10
    "Segment Not Present",		//	11
    "Stack Exception",			//	12
    "General Protection",		//	13
    "Page Fault",			//	14
    "*reserved*",			//	15
    "Floating-point error",		//	16
    "Alignment Check",			//	17
    "Machine Check",			//	18
    "*reserved*", 			//	19
    "*reserved*", 			//	20
    "*reserved*", 			//	21
    "*reserved*", 			//	22
    "*reserved*", 			//	23
    "*reserved*", 			//	24
    "*reserved*", 			//	25
    "*reserved*", 			//	26
    "*reserved*", 			//	27
    "*reserved*", 			//	28
    "*reserved*", 			//	29
    "*reserved*", 			//	30
    "*reserved*" 			//	31
};           


static struct TSS *exception_tss[32];

void create_exception_handler(int int_num, uint32 code );
void create_real_exception_handler(int int_num, uint32 code );

void init_exceptions()
{
  dmesg( "setting up exception handlers.\n");

  create_real_exception_handler( 0,  (uint32)__exception_handler0 );
  create_real_exception_handler( 1,  (uint32)__exception_handler1 );
  create_real_exception_handler( 2,  (uint32)__exception_handler2 );
  create_real_exception_handler( 3,  (uint32)__exception_handler3 );
  create_real_exception_handler( 4,  (uint32)__exception_handler4 );
  create_real_exception_handler( 5,  (uint32)__exception_handler5 );
  create_real_exception_handler( 6,  (uint32)__exception_handler6 );
  create_exception_handler( 7,  (uint32)__exception_handler7 );
  create_real_exception_handler( 8,  (uint32)__exception_handler8 );
  create_real_exception_handler( 9,  (uint32)__exception_handler9 );
  create_real_exception_handler( 10, (uint32)__exception_handlerA );
  create_real_exception_handler( 11, (uint32)__exception_handlerB );
  create_real_exception_handler( 12, (uint32)__exception_handlerC );
  create_real_exception_handler( 13, (uint32)__exception_handlerD );
  create_real_exception_handler( 14, (uint32)__exception_handlerE );
  create_real_exception_handler( 15, (uint32)__exception_handlerF );
  create_real_exception_handler( 16, (uint32)__exception_handler10 );
  create_real_exception_handler( 17, (uint32)__exception_handler11 );
  create_real_exception_handler( 18, (uint32)__exception_handler12 );
  create_real_exception_handler( 19, (uint32)__exception_handler13 );
  create_real_exception_handler( 20, (uint32)__exception_handler14 );
  create_real_exception_handler( 21, (uint32)__exception_handler15 );
  create_real_exception_handler( 22, (uint32)__exception_handler16 );
  create_real_exception_handler( 23, (uint32)__exception_handler17 );
  create_real_exception_handler( 24, (uint32)__exception_handler18 );
  create_real_exception_handler( 25, (uint32)__exception_handler19 );
  create_real_exception_handler( 26, (uint32)__exception_handler1A );
  create_real_exception_handler( 27, (uint32)__exception_handler1B );
  create_real_exception_handler( 28, (uint32)__exception_handler1C );
  create_real_exception_handler( 29, (uint32)__exception_handler1D );
  create_real_exception_handler( 30, (uint32)__exception_handler1E );
  create_real_exception_handler( 31, (uint32)__exception_handler1F );
}


void create_real_exception_handler(int int_num, uint32 code )
{
	struct TSS *tss;
	uint32 gdt;

	   tss = memory_alloc( sizeof( struct TSS ) / PAGE_SIZE + 1 );
	
    	   gdt = gdt_new_segment( (uint32)(tss) , sizeof(struct TSS),
                                   G_TSS32 | G_SYSTEM | G_PRESENT, 
			  					   G_AVAIL ,  G_DPL0 );

  exception_tss[ int_num ] = tss;	   

  tss->previous = 0;
  tss->eflags = EFLAG_BASE;

  tss->eax = 0;
  tss->ebx = 0;
  tss->ecx = 0;
  tss->edx = 0;
  tss->esi = 0;
  tss->edi = 0;
  tss->ebp = 0;
  
  tss->trapflag = 0;
  tss->iomapbase = 0;


      tss->cs = system_CS();
      tss->es = system_DS();
      tss->ds = system_DS();
      tss->fs = system_DS();
      tss->gs = system_DS();

      tss->ss  = system_DS();
      tss->ss0 = system_DS() | 0;
      tss->ss1 = system_DS() | 1;
      tss->ss2 = system_DS() | 2;

  tss->cr3 = (uint32)(&page_directory);

  tss->esp0 = (uint32)memory_alloc(1) + PAGE_SIZE;
  tss->esp = tss->esp0;
      
  tss->esp1 = 0;
  tss->esp2 = 0;

  tss->ldt = 0;

  tss->eip = code;
	
  set_interrupt( int_num,  IDT_PRESENT | IDT_TASK, gdt, 0 ,  IDT_DPL0 );
}

void create_exception_handler(int int_num, uint32 code )
{
  set_interrupt( int_num,  
				 IDT_PRESENT | IDT_32 | IDT_INT, 
				 system_CS() , code, IDT_DPL0 );
}




// ****************************************************************

// ****************************************************************



/*
 * device_not_available
 *
 *
 * 	exception 0x7 will be generated when the FPU or floating 
 * 	point instructions are executed. This also includes the 
 * 	SSE,MMX,XMM,SSE2 instruction set.  
 *
 * 	At this point, the course of action is decided by:
 *
 * 	current_thread()->math_state
 *
 * 			-1 = init math and return
 * 			 0 = restore the saved math
 * 			 1 = this should never be 1 in an exception
 * 			     because this means we have already
 * 			     initialized and restored the math.
 * 
 * 	Each thread has a 512 byte buffer into which to save the
 * 	processor state.
 * 
 */


/// \todo Make sure that it's safe to restore. 

void device_not_available()
{
	struct thread *t;
	struct thread_data *data;

	t = current_thread();
    data = ((struct thread_data*)(t->data));

	switch (data->math_state)
	{
	  case -1: clean_fpu( t );
		   break;
	  case 0: restore_fpu( t );
		  break;
	  case 1: 
		  if ( (cr0() & FPU_TS) == FPU_TS ) release_fpu_trap();
		  break;
	}
}




// *******************  REAL EXCEPTIONS ******************

int display_exception( int num, struct TSS *previous )
{
  uint32 address;
  uint32 flags;
  uint32 *d;
  uint32 fadd;
  struct thread_data *t_data = NULL;

  fadd = cr2();

  
  dmesg("%!\n*************** EXCEPTION *****************\n" );

  // Useful information
  dmesg("%!Please send me this information:\n");
  
  if ( current_process() != NULL )
  {
	if ( current_thread() != NULL )
     dmesg("%!  process.thread: %s.%s\n", current_process()->name, current_thread()->name );
	else 
     dmesg("%!         process: %s\n", current_process()->name );

    dmesg("%!       exception: %s\n", exceptions[num] );
    dmesg("%!              vm: %x\n", current_process()->map->pde );
    dmesg("%!             cr2: %x\n", fadd );
  }

    dmesg("%!          cs:eip = [%x:%x]\n", previous->cs, previous->eip);
    dmesg("%!          ss:esp = [%x:%x]\n", previous->ss, previous->esp);
  


  dmesg("%!You don't need to send me this information:\n");
  
  // ------------- safe information -----------------
  dmesg("%!CR3: %x  TSS CR3: %x\n", cr3(), previous->cr3 );
  dmesg("%!CR0: %x      CR4: %x   EBP: %x\n", cr0(), cr4(), previous->ebp );

  
  dmesg("%! EAX=%x  EBX=%x  ECX=%x EDX=%x\n", previous->eax, previous->ebx, previous->ecx, previous->edx );
  dmesg("%!  GS=%x   ES=%x   FS=%x  DS=%x\n", previous->gs, previous->es, previous->fs, previous->ds );
  dmesg("%!ESP0=%x ESP1=%x ESP2=%x ESP=%x\n", previous->esp0, previous->esp1, previous->esp2, previous->esp );
  dmesg("%! SS0=%x  SS1=%x  SS2=%x  SS=%x\n", previous->ss0, previous->ss1, previous->ss2, previous->ss );



  dmesg("%!EFLAGS = %x  (", previous->eflags);
  if ( (previous->eflags & EFLAG_BASE) 		!= 0 ) dmesg("%! BASE ");
  if ( (previous->eflags & EFLAG_INTERRUPT) != 0 ) dmesg("%! INT ");
  if ( (previous->eflags & EFLAG_IOPL3) 	!= 0 ) dmesg("%! IOPL3 ");
  dmesg("%! )\n");


  // Now delve into the system.
  
  if ( current_thread() == NULL ) return 0;		// Weird but maybe.

  t_data = (struct thread_data*)(current_thread()->data);
  if ( t_data == NULL ) return 0;
  

	  dmesg("%! KERNEL STACK: [%x:%x]", t_data->stack_kernel - PAGE_SIZE * sK_PAGES_KERNEL, t_data->stack_kernel );
	  dmesg("%!   USER STACK: [%x:%x]\n", t_data->stack_user - PAGE_SIZE * t_data->stack_pages, t_data->stack_user );

  // stack trace -------------------------------------
  
	dmesg("%!stack trace:\n");
	 
    flags = 0;
	for ( address=previous->ebp; 
		 (
		  	((address < sK_CEILING) && (address>sK_BASE))
				 ||
			 (
			  (address > (t_data->stack_kernel - PAGE_SIZE * sK_PAGES_KERNEL)) &&
			  (address <  t_data->stack_kernel)
			 ) 
		 )
		 ;
	 	 address=fadd)
	{
		if ( flags++ == 10 ) break;
		// Get the return address  
		fadd=page_data_get((uint32*)(previous->cr3), address+4);
		if ( (flags % 2) == 0 ) dmesg("%!  %x=%x\n", address+8, fadd);
						   else dmesg("%!  %x=%x", address+8, fadd);
		// Get the next stack pointer
		fadd=page_data_get((uint32*)(previous->cr3), address);    
	}


	// Some useful information about the page fault, if it was a page fault.
	  
  if ( num == 14 )
  {
	d = current_process()->map->pde;
 	flags   = d[ page_table_of(fadd) ] & 0xFFF;
	address = d[ page_table_of(fadd) ] & ~0xFFF;
	if ( (flags & PAGE_PRESENT) == PAGE_PRESENT )
	{
		d = (uint32*)address;
 	    flags   = d[ page_entry_of( fadd ) ] & 0xFFF;
        address = d[ page_entry_of( fadd ) ] & ~0xFFF;

		if ( (flags & PAGE_PRESENT) != PAGE_PRESENT )
			dmesg("%!%s\n","PAGE ENTRY NOT ASSIGNED." );
	}
	else dmesg("%!%s\n","PAGE TABLE NOT ASSIGNED.");

  }
 
  
  // -------------------------------------------------


  return 0;
}



int crash_offending( uint32 num, struct TSS* previous )
{
   struct process *proc	= current_process();
   struct thread *tr	= current_thread();

   // Kernel caused an exception?
		if ( (tr == NULL) || (proc==NULL) )
		{
		  dmesg("%!%s\n","Uhoh. Kernel caused an exception!");
		  while (1==1) {}
		}
	
	set_thread_state( tr, THREAD_CRASHED );	// Thread is crashed.
	
	// Already crashed?
	if ( proc->state == PROCESS_CRASHED )
	{
	  dmesg("%!%s\n","Weird. A crashed process crashed again?");
	  dmesg("%!%s.%s\n",proc->name,tr->name);
	  while (1==1) {}
	}

	if ( proc->state != PROCESS_CRASHING ) 
				proc->state = PROCESS_CRASHING;

	//  put our thread into a dead location...

	
	previous->eip = (uint32)__loop;
	previous->cs  = system_CS();
	previous->ds  = system_DS();
	previous->ss  = system_DS();
	previous->ss0  = system_DS();
	previous->ss1  = system_DS() | 1;
	previous->ss2  = system_DS() | 2;
	previous->esp = previous->esp0-4;
			// giving a crashed thread system level privileges...

   return 0;
}


int general_exception( int num, struct TSS* tss )
{
	struct process *proc = current_process();
	if ( proc == NULL ) return -1;

	if ( (proc->flags & IOPRIV) != 0 )
	{
		tss->eflags |= EFLAG_IOPL3;
		return 0;
	}

	return -1;
}

/*  real_exceptions are their own non-interruptable TASKS.  */

int real_exception( uint32 num )
{
	uint16 prev_gdt;
	struct TSS *previous;
	int rc = -1;


	prev_gdt = exception_tss[num]->previous;
	previous = (struct TSS*)gdt_get_offset( prev_gdt );

	switch ( num )
	{
	  case 13: rc = general_exception( num, previous );
	  		   break;
	}
	
	if ( rc == 0 ) return 0;

   	display_exception( num, previous );
	crash_offending( num, previous );

	//while ( inb(0x60) != 1 ) {};
	//while ( inb(0x60) == 1 ) {};

  return 0;
}

