#include <types.h>
#include <kernel.h>


/**  \defgroup SETJMP  setjmp/longjmp  
 *
 */

/** @{ */

// This will work on any x86 machine in protected mode.

int smk_setjmp( smk_jmpbuf envp )
{
	__asm__ ( 
		"mov %0, %%eax\n"
		"movl %%ebp, (%%eax)\n"
		"movl %%ebx, 0x4(%%eax)\n"
		"movl %%edi, 0x8(%%eax)\n"
		"movl %%esi, 0xC(%%eax)\n"

		"movl %%ebp, %%ebx\n"
		"add $0x4, %%ebx\n"
		"movl (%%ebx), %%ecx\n"
		"movl %%ecx, 0x10(%%eax)\n"
	 : 
	 : "p" (&(envp[0].regs))
	 : "eax", "ebx", "ecx"
	);	

	return 0;
}

void smk_longjmp( smk_jmpbuf envp, int value )
{
	__asm__ __volatile__ 
	(
		"mov %1, %%eax\n"
	 	"mov %0, %%esp\n"

		"mov (%%esp), %%ebp\n"
		"mov 0x4(%%esp), %%ebx\n"
		"mov 0x8(%%esp), %%edi\n"
		"mov 0xC(%%esp), %%esi\n"
		"mov 0x10(%%esp), %%ecx\n"

		"leave\n"
		"popl %%edx\n"
		"pushl %%ecx\n"
		
		"ret\n"
	 
	 :
	 : "p" (&(envp[0].regs)), "g" (value)
	 : "eax", "ecx", "esp"
	);	
}


/** @} */

