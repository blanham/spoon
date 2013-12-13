#include <inttypes.h>
#include <alloc.h>
#include <atomic.h>

#include <eflags.h>

#include <process.h>
#include <threads.h>

#include <misc.h>
#include <fpu.h>

#include <paging.h>
#include <ualloc.h>

#include <physmem.h>

#include <sysenter.h>

#include <stats.h>

#include <apic.h>

// -----------------------------------------------
//  htable key and compare functions

int threads_key( void *td )
{
	struct thread *t = (struct thread*)td;
	int ans = t->tid * 100;
	if ( ans < 0 ) ans = -ans;
	return ans;
}

int threads_compare( void *ad, void *bd )
{
	struct thread *a = (struct thread*)ad;
	struct thread *b = (struct thread*)bd;

	if ( (b->tid != -1) )
	{
		if ( a->tid < b->tid ) return -1;
		if ( a->tid > b->tid ) return 1;
		return 0;
	}
	
	return strncmp( a->name, b->name, NAME_LENGTH );
}


// -----------------------------------------------

#include <scheduler.h>

void set_thread_state( struct thread *t, unsigned int state )
{
	if ( t->state == state ) return;
	
	if ( t->state == THREAD_RUNNING ) dequeue( t );
	if (    state == THREAD_RUNNING ) queue( t );
	
	t->state = state;
}


// -----------------------------------------------

int next_tid( struct process *proc )
{
	int tid;
	struct thread temp;
				   temp.name[0] = 0;
				   temp.tid = -1;

		
		do
		{
			if ( temp.tid != -1 )
			{
				proc->last_tid += 1;
				if ( proc->last_tid < 0 ) proc->last_tid = 1;
			}
	
			temp.tid = proc->last_tid;
		}
		while ( htable_retrieve( proc->threads_htable, &temp ) != NULL );

		tid = proc->last_tid++;

	return tid;
}


// -----------------------------------------------


struct thread* get_thread( struct process *proc, int tid )
{
	struct thread temp;
				   temp.name[0] = 0;
				   temp.tid = tid;

		
	return (struct thread*)htable_retrieve( proc->threads_htable, &temp );
}


static inline void pushl( struct thread *tr, uint32_t value )
{
	uint32_t *mem;
	tr->stack = tr->stack - 4;
	mem = (uint32_t*)(tr->stack);
	mem[0] = value;
}





int new_thread( int type,
	       	    struct process *proc, 
		   	    struct thread *parent, 
		        const char name[NAME_LENGTH],
			    uint32_t entry,
			    int priority,
			    uint32_t data1,
			    uint32_t data2,
			    uint32_t data3,
			    uint32_t data4 )
{
	struct thread *tr = (struct thread*)malloc( sizeof(struct thread) );
	if ( tr == NULL ) return -1;

	strncpy( tr->name, name, NAME_LENGTH );

	tr->tid 	= next_tid( proc );
	tr->state 	= THREAD_SUSPENDED;
	tr->process = proc;

	tr->sched_state	=	-1;




	// architecture ...
	// 

		tr->stack_user = page_find_free( proc->map, 
										    sK_BASE,
											sK_CEILING,
											sK_PAGES_THREAD + 1 );

		tr->stack_user += (sK_PAGES_THREAD + 1) * PAGE_SIZE;

		// We're leaving a guard page at the bottom (and,
		// therefore, the top) of every thread stack.

			user_ensure( proc, 
						 tr->stack_user - (sK_PAGES_THREAD * PAGE_SIZE),
						 sK_PAGES_THREAD );


		tr->stack_kernel = (uint32_t) memory_alloc( sK_PAGES_KERNEL );
		tr->stack_kernel += sK_PAGES_KERNEL * PAGE_SIZE;
	     

	tr->cpu = -1;
	tr->cpu_affinity = -1;				// No preference.
		
	tr->math_state = -1;
	tr->math = memory_alloc( 1 );


	tr->stack = tr->stack_kernel;

	// ------------
   
	if ( type == 1 )
	{
		// kernel thread
		pushl( tr, GDT_SYSTEMDATA );
		pushl( tr, tr->stack_user );
		pushl( tr, EFLAG_BASE | EFLAG_INTERRUPT  );
		pushl( tr, GDT_SYSTEMCODE );
	}
	else
	{
		// user thread
		pushl( tr, GDT_USERDATA | 3 );
		pushl( tr, tr->stack_user );
		pushl( tr, EFLAG_BASE | EFLAG_INTERRUPT  );
		pushl( tr, GDT_USERCODE | 3 );
	}

	pushl( tr, entry );


	pushl( tr, GDT_USERDATA | 3 );	// gs
	pushl( tr, GDT_USERDATA | 3 );	// fs
	pushl( tr, GDT_USERDATA | 3 );	// ds
	pushl( tr, GDT_USERDATA | 3 );	// es

	pushl( tr, data1 );				// eax
	pushl( tr, data3 );				// ecx
	pushl( tr, data4 );				// edx
	pushl( tr, data2 );				// ebx

	pushl( tr, 0 );					// temp esp

	pushl( tr, 0 );					// ebp
	pushl( tr, 0 );					// esi
	pushl( tr, 0 );					// edi


	FAMILY_INIT( tr );
	LIST_INIT( tr );


	// ....

	if ( proc->threads == NULL )
	{
		proc->threads = tr;
	}
	else
	{
		if ( parent == NULL )
		{
			family_add_sibling( &(parent->family_tree), &(tr->family_tree) );
			list_insertAfter( &(proc->threads->list), &(tr->list) );
		}
		else
		{
			family_add_child( &(parent->family_tree), &(tr->family_tree) );
			list_insertAfter( &(proc->threads->list), &(tr->list) );
		}
	}

	htable_insert( proc->threads_htable, tr );

	proc->thread_count += 1;

	return tr->tid;
}



#include <cpu.h>
#include <tss.h>

extern uint32_t __switch_thread( uint32_t stack );


void set_switch_stack( uint32_t st )
{
	cpu[ CPUID ].scheduler_stack = st;
}

uint32_t get_switch_stack()
{
	return cpu[ CPUID ].scheduler_stack;
}



// -----------

int exec_thread( unsigned int cpu_id, struct thread *t, 
				 unsigned int milliseconds )
{
	assert( t != NULL );
	
	set_fpu_trap();
	
	set_map( t->process->map );

	cpu[ cpu_id ].sched.running = 1;			// Marked as running before
	cpu[ cpu_id ].sched.current_thread = t;		// Mark the thread.
	
	release_spinlock( &(cpu[ cpu_id ].sched.lock_scheduler) ); 
			// Other CPU's can register their need to mess with this CPU's tables.
	

					
		do
		{
		  	sysenter_set_esp( t->stack_kernel );
	
			cpu[ cpu_id ].system_tss->esp0 = t->stack_kernel;
			cpu[ cpu_id ].system_tss->esp  = t->stack;
			cpu[ cpu_id ].system_tss->cr3  = (uint32_t)t->process->map;

			set_apic_distance( cpu_id, milliseconds );
	
			stats_time( cpu_id, &(cpu[ cpu_id ].st_schedulerTime) ); // Scheduler time.

			t->stack = __switch_thread( t->stack );

			stats_time_start( &(cpu[ cpu_id ].st_schedulerTime) ); // Scheduler
		   
		} while ( cpu[cpu_id].sched.locked == 1 ); // in case...

	// If math was used....
	if ( t->math_state > 0 ) save_fpu( t );
	
	cpu[ cpu_id ].sched.current_thread = NULL;
	cpu[ cpu_id ].sched.running = 0;
			// WARNING: Don't use the *t pointer anymore after this.

			// Synchronization point occurs here. Table messing. 

	acquire_spinlock( &(cpu[cpu_id].sched.lock_scheduler) );

	
	return 0;
}




