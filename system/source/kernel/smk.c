#include <process.h>
#include <assert.h>
#include <threads.h>


#include <cpu.h>
#include <time.h>

struct process *smk_process = NULL;
struct thread  *smk_idle[MAX_CPU];


void idle()
{
	while (1==1) 
	{
		((char*)0xB8000)[0]++;
		asm ("hlt");
	}
}


void init_smk()
{
	int i;
		
	smk_process = new_process( "smk" );

	assert( smk_process != NULL );

	insert_process( 0, smk_process );
	
	for ( i = 0; i < MAX_CPU; i++ )
			smk_idle[i] = NULL;
}



/** This function spawns an smk_idle thread for the CPU which calls
 * it. So if there are 2 CPU's in a system, they will both call this
 * function and there will be 2 idle threads after all is said and
 * done.
 */
void load_smk( unsigned int cpu_id )
{
	int tid;

	struct process* proc = checkout_process( smk_process->pid, WRITER );

	assert( proc != NULL );

	tid = new_thread( 1, proc, NULL, "smk_idle", (uint32_t)idle, 0, 1,2,3,4 );

	smk_idle[ cpu_id ] = get_thread( proc, tid );

	assert( smk_idle[ cpu_id ] != NULL );

	commit_process( proc );
}


