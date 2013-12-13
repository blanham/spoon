#include <types.h>
#include <conio.h>
#include <strings.h>
#include <cpu.h>
#include <dmesg.h>
#include <params.h>

#include <random.h>
#include <irq.h>
#include <process.h>

#include <sem.h>
#include <ioperms.h>
#include <shmem.h>

#include <console.h>
#include <scheduler.h>

#include <switching.h>


#include <smk.h>

extern void arch_init_modules();
extern struct thread* smk_idle;

int kernel( char *cmd_line )
{
		
  init_irqs();

  init_processes();

  init_semaphores();

  init_ioperms();
  
  init_random();

  init_consoles();

  init_shmem();
	
  sched_init();
  
  init_switching();
  
  init_smk();
 
  arch_init_modules();

  
	  dmesg("%s\n",     "spoon microkernel" ); 
	  dmesg("%s%s\n",   "version: ", _VERSION_STR_ ); 
	  dmesg("%s%s\n\n", "architecture: ", _ARCH_STR_ ); 
	  if ( cmd_line != NULL ) 
	  {
		  dmesg("%s%s\n", "command line: ", cmd_line ); 
	  }
  
  
  start_switching();

  dmesg("%s\n","back in kernel.");
	  
  sched_shutdown();
  
  while (1==1) { }
  
  return 0;
}


// Design is the hardest part of programming.

