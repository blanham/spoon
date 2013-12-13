

/**  \defgroup SYSENTER  Sysenter hook into the kernel  
 *
 */

/** @{ */

int  _sysenter( unsigned int r1, unsigned int r2, unsigned int r3, 
				unsigned int r4, unsigned int r5, unsigned int r6 )
{
  int answer;

		//  the presence of the eax here is actually a place
		//  holder in which the result of the syscall will be
		//  returned.  That's also why we're popping it into eax
		//  afterwards.

  asm (
		  "  pushl %1\n"
		  "  pushl %2\n"
		  "  pushl %3\n"
		  "  pushl %4\n"
		  "  pushl %5\n"
		  "  pushl %6\n"
		  "  pushl %%eax\n"
		  
		  "  mov $sysentry_return, %%edx\n"
		  "  mov %%esp, %%ecx\n"
		  
		  "  sysenter\n"
          
	  	"sysentry_return:\n"
		  "  popl %%eax\n"

		  "  add $0x18, %%esp\n"
		  "  mov %%eax, %0\n"
		     
		  
		  : "=g" (answer) 
		  : "g" (r1), "g" (r2), "g" (r3), "g" (r4), "g" (r5), "g" (r6)
		  : "eax","edx","ecx"
	  );

  return answer;	
}


/** @} */
