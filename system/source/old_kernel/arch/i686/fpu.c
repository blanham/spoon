#include <types.h>
#include <dmesg.h>
#include <macros.h>
#include <conio.h>
#include <threads.h>


#include "fpu.h"
#include "thread_data.h"


void set_fpu_trap()
{
	__asm__	(
		 	 "mov %%cr0, %%eax;\n"
			 "and  %0, %%eax;\n"
		     "or   %1, %%eax;\n"
		     "mov %%eax, %%cr0;\n"
	
	 :
	 : "g" (FPU_CLEAN), "g" (FPU_TS | FPU_MP)
	 : "eax"
	);
}

void release_fpu_trap()
{
	__asm__	( "clts" );
}



void clean_fpu( struct thread *t )
{
	release_fpu_trap();
	asm ("finit");
	((struct thread_data*)(t->data))->math_state = 1;
}

void save_fpu( struct thread *t )
{
	struct thread_data *data = t->data;
	release_fpu_trap();

	asm ( " movl %0, %%eax; \
	        FXSAVE (%%eax) " 
		 : 
		 : "g" ( data->math ) 
		 : "eax" );

	data->math_state = 0;

	set_fpu_trap();
}

void restore_fpu( struct thread *t )
{
	struct thread_data *data = t->data;
	release_fpu_trap();

	asm ( " movl %0, %%eax; \
			FXRSTOR (%%eax) " 
		: 
		: "g" (data->math) 
		: "eax" );

	data->math_state = 1;
}



void init_fpu()
{
	dmesg( "initialising the FPU.\n" );
	asm ("finit");


	dmesg( "enabling the FXRSTOR and FXSAVE calls.\n");

	asm ( "  movl %%cr4, %%eax;\n "
		  "  orl  %0, %%eax;\n    "
		  "  movl %%eax, %%cr4;\n "
		  :
		  : "g" ( (1<<9) )
		  : "eax"
		);
	
	// Don't set the fpu_trap.

}


