#include <atomic.h>
#include <conio.h>
#include <dmesg.h>
#include <macros.h>
#include <smk.h>
#include <smk_idle.h>
#include <process.h>

#include <cpu.h>


/*
 *	smk_idle
 *
 *		this thread is run by the scheduler when all userland
 *		threads are sleeping or domant.
 *
 *		I do this because this thread allows interrupts and 
 *		irqs to keep occuring. If I didn't, the scheduler would 
 *		get stuck in an infinite, unbreakable loop resembling my
 *		daily life.
 *
 */

// ***********************************************************

struct thread *smk_idle;



int idle(void* data);


// ***********************************************************


void init_smk_idle()
{
	int tid;
	
	dmesg("creating the idle thread.");

	// Set up the smk idle thread. ( see above )

	tid = new_thread( 1,  smk_process,
				    smk_process->threads,
				    "idle",
				    (uint32)idle,
				    1,
				    0,
				    0,
				    0,
				    0
				   );

	smk_idle = find_thread_with_id( smk_process, tid );
	set_thread_state( smk_idle, THREAD_SYSTEM );
}



int idle(void* data)
{
	while (1==1) halt_cpu();

	return 0; 
}



