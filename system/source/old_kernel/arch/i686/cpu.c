#include <types.h>
#include <dmesg.h>
#include <strings.h>
#include <params.h>

#include "misc.h"

// ----------------------------------------------

void cpu_feature_required( char *feature )
{
	dmesg("%!%s\n", feature );
	dmesg("%!%s\n","FAILURE: This CPU feature is required for successful operation of the spoon microkernel.");
	dmesg("%!%s\n","\nkernel halted.");
	while (1==1) {}
}



void bitcheck(uint32 data, int bit, int required, char *message)
{
  int i;
  
		dmesg("%s%s", "   ", message );
	
		i = 50 - strlen(message);
		while ( i-- > 0 ) dmesg( "%s", " " );
		

	if ( (data & (1<<bit)) != 0 ) 
	{
		dmesg("%s\n","present.");
	}
	else
	{
		dmesg("%s\n","MISSING.");
		if ( required == 1 ) cpu_feature_required( message );
	}
			
}


int init_cpu()
{
	uint32 eax, ebx, ecx, edx;

	dmesg("checking CPU features.\n");
	
	asm ( " pusha;			 \
			movl $1, %%eax; \
			cpuid;			 \
			mov %%eax, %0;	 \
			mov %%ebx, %1;	 \
			mov %%ecx, %2;	 \
			mov %%edx, %3;	 \
			popa; 			 "
		  : "=g" (eax), "=g" (ebx), "=g" (ecx), "=g" (edx)
		  : 
		  : "eax", "ebx", "ecx", "edx"
		);


	bitcheck(edx,0,1,"Virtual 8086");
	bitcheck(edx,3,1,"Page Size Extensions");
	bitcheck(edx,4,1,"Time Stamp Counter");
	bitcheck(edx,5,1,"RDMSR and WRMSR Instructions");
	bitcheck(edx,11,1,"SYSENTER/SYSEXIT");
	bitcheck(edx,13,1,"PTE Global Bit");
	bitcheck(edx,23,0,"MMX");
	bitcheck(edx,24,1,"FXSAVE/FXRSTOR");
	bitcheck(edx,25,0,"SSE");
	bitcheck(edx,26,0,"SSE2");


	
	return 0;
}




