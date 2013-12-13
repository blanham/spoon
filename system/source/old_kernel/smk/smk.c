#include <conio.h>
#include <dmesg.h>
#include <macros.h>
#include <process.h>
#include <console.h>

#include <smk_gc.h>
#include <smk_idle.h>

struct process *smk_process = NULL;



void init_smk()
{
	dmesg( "creating the smk process.\n" );

	smk_process = new_process( "smk");
	insert_process( -1, smk_process );

	/// \todo make safe, really.
	
		attach_console( smk_process, 0 );
		smk_process->state = PROCESS_SYSTEM;	
	
	init_smk_idle();
	init_smk_gc();
}



