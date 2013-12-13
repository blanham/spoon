#include <configure.h>
#include <dmesg.h>
#include <macros.h>
#include <atomic.h>
#include <conio.h>
#include <alloc.h>
#include <process.h>
#include <threads.h>
#include <paging.h>
#include <scheduler.h>
#include <smk.h>
#include <smk_idle.h>
#include <smk_gc.h>
#include <memory.h>
#include <time.h>
#include <irq.h>
#include <switching.h>
#include <params.h>


#include "io.h"
#include "eflags.h"
#include "time.h"
#include "fpu.h"
#include "tss.h"
#include "misc.h"
#include "int.h"
#include "irq.h"
#include "gdt.h"
#include "sysenter.h"
#include "thread_data.h"


/*
 *  This file provides the switching facilities for thread execution
 *  and system context information.
 *
 *
 */


extern void __hard_scheduler();
extern void __soft_scheduler();

// ********* VARIABLES AND STATIC VARIABLES ***********************


static volatile int	_switch_inside			=	0;

static volatile uint32 _switch_stack	= 0;
static struct TSS* _switch_tss			= NULL;
static uint32 _switch_gdt				= 0;

static uint32 _kernel_gdt					= 0;
static struct TSS* _kernel_tss				= 0;

// ******** FORWARD DECLARATIONS **********************************

int	switch_exec_thread( struct thread * );
int	switch_wake_thread( struct thread * );
int	switch_irq_thread( struct thread * );
int	switch_crashed_thread( struct thread * );
int	switch_finished_thread( struct thread * );

void   set_switch_stack( uint32 st );
uint32 get_switch_stack();

extern void __switch_entry();
extern uint32 __switch_thread( uint32 stack );




// *************** INITIALIZATION ROUTINE***************************

int init_switching()
{
	_switch_inside			=	0;
	g_switchingProcess	= 	NULL;
	g_switchingThread	=	NULL;
	_switch_stack 	=	0;
	_switch_tss	=	NULL;
 
	dmesg( "initializing thread switching.\n" );

	set_interrupt( 0x20,  IDT_PRESENT | IDT_32 | IDT_INT,
					system_CS() , (uint32)__hard_scheduler, IDT_DPL0 );


	set_interrupt( 0x30,  IDT_PRESENT | IDT_32 | IDT_INT,
					system_CS() , (uint32)__soft_scheduler, IDT_DPL3 );
	
	unmask_irq(0);
	return 0;	
}


// *************** STARTUP ROUTINE ********************************


int start_switching()
{
	vmmap *map;

	// first load a TSS into the TR so that we can return later.
	_kernel_tss = memory_alloc( sizeof( struct TSS ) / PAGE_SIZE + 1 );
	
  	_kernel_gdt = gdt_new_segment( (uint32)(_kernel_tss) , sizeof(struct TSS),
                                          G_TSS32 | G_SYSTEM | G_PRESENT, 
                                          G_AVAIL ,  G_DPL0 );

      ltr( _kernel_gdt );
	// Done, we're safe.

	
	_switch_tss = memory_alloc( sizeof( struct TSS ) / PAGE_SIZE + 1 );
	
   	_switch_gdt = gdt_new_segment( (uint32)(_switch_tss) , sizeof(struct TSS),
                               G_TSS32 | G_SYSTEM | G_PRESENT, 
                               G_AVAIL ,  G_DPL3);

  _switch_tss->eflags = EFLAG_BASE;

  _switch_tss->eax = 0;
  _switch_tss->ebx = 0;
  _switch_tss->ecx = 0;
  _switch_tss->edx = 0;
  _switch_tss->esi = 0;
  _switch_tss->edi = 0;
  _switch_tss->ebp = 0;
  
  _switch_tss->trapflag = 0;
  _switch_tss->iomapbase = 0;


      _switch_tss->cs = system_CS();
      _switch_tss->es = system_DS();
      _switch_tss->ds = system_DS();
      _switch_tss->fs = system_DS();
      _switch_tss->gs = system_DS();

      _switch_tss->ss  = system_DS() ;
      _switch_tss->ss0 = system_DS() | 0;
      _switch_tss->ss1 = system_DS() | 1;
      _switch_tss->ss2 = system_DS() | 2;

      
  	map = new_vmmap();
	
  _switch_tss->esp1 = 0;
  _switch_tss->esp2 = 0;
  _switch_tss->ldt = 0;
  _switch_tss->cr3 = (uint32)( map->pde );

  
  _switch_tss->esp  = (uint32)memory_alloc( 1 ) + 1 * PAGE_SIZE;
  _switch_tss->esp0 = _switch_tss->esp;
  _switch_tss->eip  = (uint32)__switch_entry;

  set_switch_stack( _switch_tss->esp );

  call_task( _switch_gdt );

  
  
  return 0;
}



// ****************************************************************


void set_switch_stack( uint32 st )
{
	_switch_stack = st;
}

uint32 get_switch_stack()
{
	return _switch_stack;
}


int inside_scheduler()
{
	if ( _switch_inside == 1 ) return 1;
	return 0;
}



// ****************************************************************


void switcher()
{
	int rc;
	set_cpu_flags(  cpu_flags() & ~EFLAG_NESTED_TASK );

	
	_switch_inside = 1;
	  
	   rc = scheduler();
	   
	_switch_inside = 0;


	// return back to the caller .. the kernel.

	_switch_tss->previous = _kernel_gdt;
	
	set_cpu_flags(  cpu_flags()  | EFLAG_NESTED_TASK );
	
}


// *******  SWITCHING ROUTINES USED BY SWITCH_THREAD **************


// It won't reload the CR3 if the new thread is executing in the same
// context.

int switch_exec_thread( struct thread *t )
{
	int ans;
	unsigned int count = 0;
	uint32 *stack;
	struct thread_data *data;
	static uint32 previous_cr3 = 0;
	

	g_switchingProcess = t->process;
	g_switchingThread  = t;

	data = (struct thread_data*) t->data;
	
			// Save the TLB reloading where possible.
	if ( (previous_cr3 != (uint32)(g_switchingProcess->map->pde)))
	{
		previous_cr3 = (uint32)(g_switchingProcess->map->pde);
		set_vmmap( g_switchingProcess->map );
	}

	
	set_fpu_trap();

	// -------  TLS set up

	if ( g_switchingProcess->tls_location != NULL )
	   *( g_switchingProcess->tls_location) = g_switchingThread->tls;
	
	do
	{
	  	sysenter_set_esp( data->stack_kernel );
  		_switch_tss->esp0 = data->stack_kernel;
	 	 _switch_tss->esp = data->stack;
		 _switch_tss->cr3 = (uint32)(t->process->map->pde);
		stack = (uint32*)(data->stack);


		if ( debug != 0 )
		{
			dmesg_xy( 0, 0, "(%i) %s.%s (%x,%x,%x,%x)%s\n",
						count++,
						t->process->name,
						t->name,
						stack[11],
						stack[12],
						stack[13],
						stack[14],
						" END  "
					  );
		}


	   _switch_inside = 0;
	   ans = __switch_thread( data->stack );
	   data->stack = ans;
	   _switch_inside = 1;
	   
	   
	    increment_system_time( t->process );

		
	} while ( 1==0 );  // once...

	// If math was used....
	if ( data->math_state > 0 ) save_fpu( t );

	g_switchingThread  = NULL;
	g_switchingProcess = NULL;  
	

	return 1;
}


int switch_wake_thread( struct thread *t )
{
    uint32 seconds,milliseconds;

				// PURE DORMANTS
	if ( (t->sleep_seconds == 0) && (t->sleep_milliseconds == 0) ) return 0;

				// SLEEPERS AND HIBERNATERS
	system_time( &seconds, &milliseconds );
	
	if ( t->sleep_seconds < seconds)
	{
		 t->state = THREAD_RUNNING;
		 t->sleep_seconds = 0xc007;
	 	 return switch_exec_thread( t );
	}
	
	if ( t->sleep_seconds == seconds)
	  if ( t->sleep_milliseconds <= milliseconds )
	  {
	     t->state = THREAD_RUNNING;
	     t->sleep_seconds = 0xc007;
	     return switch_exec_thread( t );
	  }

    return 0;
}


int switch_finished_thread( struct thread *t )
{
	signal_gc( t->process->pid, t->tid );
	
	return 0;
}


// ****************************************************************


int switch_thread( struct thread *t )
{
   uint32 *stack;
   struct thread_data *data = (struct thread_data*)t->data;
   
    stack = (uint32*)data->stack;
 
	// Is the process finishing...
	if ( t->process->state == PROCESS_FINISHING )
	{
		if (t->process->kernel_threads == 0 )
			t->process->state = PROCESS_FINISHED;
	}
    

	// Is the process crashing...
	if ( t->process->state == PROCESS_CRASHING )
	{
		if (t->process->kernel_threads == 0 )
			t->process->state = PROCESS_CRASHED;
	}

	if ( t->process->state == PROCESS_CRASHED ) 
	{
		return 0;
	}
	
	if ( t->process->state == PROCESS_FINISHED ) 
	{
		signal_gc( t->process->pid, -1 );
		return 0;
	}
	
	
	
	switch (t->state)
	{
		case THREAD_SYSTEM:
		case THREAD_RUNNING: 	return switch_exec_thread( t );
		case THREAD_SLEEPING:	
		case THREAD_DORMANT:    return switch_wake_thread( t );

		case THREAD_FINISHED:	return switch_finished_thread( t );
								
		default: 
			break;
	}
	
	
	return 0;
}




