#include <fpu.h>
#include <threads.h>




void clean_fpu( struct thread *t )
{
	release_fpu_trap();
	asm ("finit");
	t->math_state = 1;
}

void save_fpu( struct thread *t )
{
	release_fpu_trap();

	asm ( " movl %0, %%eax; \
	        FXSAVE (%%eax) " 
		 : 
		 : "g" ( t->math ) 
		 : "eax" );

	t->math_state = 0;

	set_fpu_trap();
}

void restore_fpu( struct thread *t )
{
	release_fpu_trap();

	asm ( " movl %0, %%eax; \
			FXRSTOR (%%eax) " 
		: 
		: "g" (t->math) 
		: "eax" );

	t->math_state = 1;
}



void load_fpu( unsigned int cpu_id )
{
	// Initialising the FPU.
	asm ("finit");


	// Enabling the FXRSTOR and FXSAVE calls.
	asm ( "  movl %%cr4, %%eax;\n "
		  "  orl  %0, %%eax;\n    "
		  "  movl %%eax, %%cr4;\n "
		  :
		  : "g" ( (1<<9) )
		  : "eax"
		);

}


