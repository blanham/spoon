#include <types.h>
#include <dmesg.h>
#include <conio.h>
#include <syscalls.h>
#include <interrupts.h>


#include "gdt.h"
#include "eflags.h"
#include "sysenter.h"

void __system_call();



int syscall_prepare( uint32 flags, uint32 eip, uint32* stack )
{
	// sysenter disabled interrupts so we should restore it if they were enabled. 
	if ( (flags & EFLAG_INTERRUPT) == EFLAG_INTERRUPT ) enable_interrupts();
	
	stack[0] = syscall( stack[1], stack[2], stack[3], 
						stack[4], stack[5], stack[6] );
	return 0;
}


void init_syscalls()
{
	dmesg( "setting up the SYSENTER msr system.\n");
	sysenter_prepare( system_CS(), 0,  (uint32)__system_call );
}



