#include <inttypes.h>
#include <tss.h>
#include <interrupts.h>
#include <gdt.h>
#include <paging.h>
#include <eflags.h>
#include <dmesg.h>
#include <misc.h>
#include <fpu.h>
#include <scheduler.h>
#include <threads.h>
#include <ioperms.h>
#include <process.h>

#include <cpu.h>

static char *exceptions[] = 
{
    "Divide-by-zero",			//	0
    "Debug Exception",			//	1
    "NMI",						//	2
    "Breakpoint",				//	3
    "INTO",						//	4
    "BOUNDS",					//	5
    "Invalid Opcode",			//	6
    "Device Not Available",		//	7
    "Double-fault",				//	8
    "Coprocessor segment overrun",	//	9
    "Invalid TSS fault",		//	10
    "Segment Not Present",		//	11
    "Stack Exception",			//	12
    "General Protection",		//	13
    "Page Fault",				//	14
    "*reserved*",				//	15
    "Floating-point error",		//	16
    "Alignment Check",			//	17
    "Machine Check",			//	18
    "*reserved*", 				//	19
    "*reserved*", 				//	20
    "*reserved*", 				//	21
    "*reserved*", 				//	22
    "*reserved*", 				//	23
    "*reserved*", 				//	24
    "*reserved*", 				//	25
    "*reserved*", 				//	26
    "*reserved*", 				//	27
    "*reserved*", 				//	28
    "*reserved*", 				//	29
    "*reserved*", 				//	30
    "*reserved*" 				//	31
};           


extern void __exception_handler0();
extern void __exception_handler1();
extern void __exception_handler2();
extern void __exception_handler3();
extern void __exception_handler4();
extern void __exception_handler5();
extern void __exception_handler6();
extern void __exception_handler7();
extern void __exception_handler8();
extern void __exception_handler9();
extern void __exception_handlerA();
extern void __exception_handlerB();
extern void __exception_handlerC();
extern void __exception_handlerD();
extern void __exception_handlerE();
extern void __exception_handlerF();
extern void __exception_handler10();
extern void __exception_handler11();
extern void __exception_handler12();
extern void __exception_handler13();
extern void __exception_handler14();
extern void __exception_handler15();
extern void __exception_handler16();
extern void __exception_handler17();
extern void __exception_handler18();
extern void __exception_handler19();
extern void __exception_handler1A();
extern void __exception_handler1B();
extern void __exception_handler1C();
extern void __exception_handler1D();
extern void __exception_handler1E();
extern void __exception_handler1F();




void create_real_exception_handler( unsigned int cpu_id, 
									int num, uint32_t code )
{
	struct TSS *tss = new_cpu_tss( 1 );
		
	cpu[ cpu_id ].exception_tss[ num ] = tss;
		
	int gdt = gdt_new_segment( cpu_id, (uint32_t)(tss), 
				  			 sizeof(struct TSS),
                             G_TSS32 | G_SYSTEM | G_PRESENT, 
			  				 G_AVAIL ,  G_DPL0 );

	tss->eip  = code;
	
	set_interrupt( cpu_id, num,  IDT_PRESENT | IDT_TASK, gdt, 0 ,  IDT_DPL0 );
}

void create_exception_handler( unsigned int cpu_id, 
							  int num, uint32_t code )
{
  set_interrupt( cpu_id, num,  
				 IDT_PRESENT | IDT_32 | IDT_INT, 
				 GDT_SYSTEMCODE , code, IDT_DPL0 );
}







void load_exceptions( int cpu_id )
{
  create_real_exception_handler( cpu_id, 0,  (uint32_t)__exception_handler0 );
  create_real_exception_handler( cpu_id, 1,  (uint32_t)__exception_handler1 );
  create_real_exception_handler( cpu_id, 2,  (uint32_t)__exception_handler2 );
  create_real_exception_handler( cpu_id, 3,  (uint32_t)__exception_handler3 );
  create_real_exception_handler( cpu_id, 4,  (uint32_t)__exception_handler4 );
  create_real_exception_handler( cpu_id, 5,  (uint32_t)__exception_handler5 );
  create_real_exception_handler( cpu_id, 6,  (uint32_t)__exception_handler6 );
  create_exception_handler( cpu_id, 7,  (uint32_t)__exception_handler7 );
  create_real_exception_handler( cpu_id, 8,  (uint32_t)__exception_handler8 );
  create_real_exception_handler( cpu_id, 9,  (uint32_t)__exception_handler9 );
  create_real_exception_handler( cpu_id, 10, (uint32_t)__exception_handlerA );
  create_real_exception_handler( cpu_id, 11, (uint32_t)__exception_handlerB );
  create_real_exception_handler( cpu_id, 12, (uint32_t)__exception_handlerC );
  create_real_exception_handler( cpu_id, 13, (uint32_t)__exception_handlerD );
  create_real_exception_handler( cpu_id, 14, (uint32_t)__exception_handlerE );
  create_real_exception_handler( cpu_id, 15, (uint32_t)__exception_handlerF );
  create_real_exception_handler( cpu_id, 16, (uint32_t)__exception_handler10 );
  create_real_exception_handler( cpu_id, 17, (uint32_t)__exception_handler11 );
  create_real_exception_handler( cpu_id, 18, (uint32_t)__exception_handler12 );
  create_real_exception_handler( cpu_id, 19, (uint32_t)__exception_handler13 );
  create_real_exception_handler( cpu_id, 20, (uint32_t)__exception_handler14 );
  create_real_exception_handler( cpu_id, 21, (uint32_t)__exception_handler15 );
  create_real_exception_handler( cpu_id, 22, (uint32_t)__exception_handler16 );
  create_real_exception_handler( cpu_id, 23, (uint32_t)__exception_handler17 );
  create_real_exception_handler( cpu_id, 24, (uint32_t)__exception_handler18 );
  create_real_exception_handler( cpu_id, 25, (uint32_t)__exception_handler19 );
  create_real_exception_handler( cpu_id, 26, (uint32_t)__exception_handler1A );
  create_real_exception_handler( cpu_id, 27, (uint32_t)__exception_handler1B );
  create_real_exception_handler( cpu_id, 28, (uint32_t)__exception_handler1C );
  create_real_exception_handler( cpu_id, 29, (uint32_t)__exception_handler1D );
  create_real_exception_handler( cpu_id, 30, (uint32_t)__exception_handler1E );
  create_real_exception_handler( cpu_id, 31, (uint32_t)__exception_handler1F );
}


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
	struct thread *t = current_thread();

	switch (t->math_state)
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




int display_exception( unsigned int cpu_id, int num, struct TSS *previous )
{
  
  dmesg("%!\n**** EXCEPTION: %s, CPU %i *****************\n",
				exceptions[num],
				cpu_id
				);


  dmesg("%!          cs:eip = [%x:%x]\n", previous->cs, previous->eip);
  dmesg("%!          ss:esp = [%x:%x]\n", previous->ss, previous->esp);
  
  dmesg("%!TSS CR3: %x\n", previous->cr3 );
  dmesg("%!EBP: %x\n", previous->ebp );
  dmesg("%!CR2: %x\n", cr2() );

  
  dmesg("%! EAX=%x  EBX=%x  ECX=%x EDX=%x\n", previous->eax, previous->ebx, previous->ecx, previous->edx );
  dmesg("%!  GS=%x   ES=%x   FS=%x  DS=%x\n", previous->gs, previous->es, previous->fs, previous->ds );
  dmesg("%!ESP0=%x ESP1=%x ESP2=%x ESP=%x\n", previous->esp0, previous->esp1, previous->esp2, previous->esp );
  dmesg("%! SS0=%x  SS1=%x  SS2=%x  SS=%x\n", previous->ss0, previous->ss1, previous->ss2, previous->ss );


  dmesg("%!EFLAGS = %x  (", previous->eflags);
  if ( (previous->eflags & EFLAG_BASE) 		!= 0 ) dmesg("%! BASE ");
  if ( (previous->eflags & EFLAG_INTERRUPT) != 0 ) dmesg("%! INT ");
  if ( (previous->eflags & EFLAG_IOPL3) 	!= 0 ) dmesg("%! IOPL3 ");
  dmesg("%! )\n");


	 
	if ( 1==1 )
	{
	 dmesg("%!stack trace:\n");
	 uint32_t flags = 0;
	 uint32_t fadd;
	 uint32_t address = previous->ebp;

	 while ( flags++ < 10 )
	 {
		// Get the return address  
		fadd=page_data_get((uint32_t*)(previous->cr3), address+4);
		if ( (flags % 2) == 0 ) dmesg("%!  %x=%x\n", address+8, fadd);
						   else dmesg("%!  %x=%x", address+8, fadd);
		// Get the next stack pointer
		address = page_data_get((uint32_t*)(previous->cr3), address);
	 }
	}


  
  return 0;
}


/*  real_exceptions are their own non-interruptable TASKS.  */

int real_exception( int num )
{
	uint16_t prev_gdt;
	struct TSS *previous;
	unsigned int cpu_id = CPUID; 
	int rc = -1;

	prev_gdt = cpu[ cpu_id ].exception_tss[num]->previous;
	previous = (struct TSS*)gdt_get_offset( cpu_id, prev_gdt );

	dmesg("%!Exception %i\n", num );

	switch ( num )
	{
	  case 13: rc = general_exception( num, previous );
	  		   break;
	}

	if ( rc == 0 ) return 0;
	
	

	display_exception( cpu_id, num, previous );

	while (1==1) 
	{
	}

  return 0;
}






