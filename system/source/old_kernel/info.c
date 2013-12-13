#include <types.h>
#include <dmesg.h>
#include <info.h>
#include <time.h>
#include <strings.h>
#include <memory.h>
#include <process.h>
#include <lengths.h>
#include <switching.h>


int info_system( struct system_information *si )
{
   memcpy( si->kernel_name, 
   		"the spoon microkernel\0", 
		 strlen("the spoon microkernel") + 1  );
   
   si->kernel_version 		= 1;
   si->cpu_speed 		= get_cpu_speed();
   si->memory_free 		= free_memory();
   si->memory_total		= total_memory();
   si->uptime 			= epoch_uptime();
   
   si->sched_length  	= 0;
   si->sched_running 	= 0;
   si->process_length	= process_length;
   si->thread_length	= thread_length;
   return 0;
}


int info_process( struct process* p, struct process_information *pi )
{
	memcpy( pi->name, p->name, OS_NAME_LENGTH );
	pi->pid 	= p->pid;
	pi->threads = p->thread_count;
	pi->state 	= p->state;
/*
	if ( p->rdtsc_sched != 0 )
	  pi->usage_cpu  = 0; //udiv64( p->rdtsc_last * 100000, p->rdtsc_sched ); 
	else pi->usage_cpu = 0;
	
*/

	pi->usage_cpu	= 0;
	
	pi->usage_kmem 	= 0;
	pi->usage_umem 	= 0;
	
	pi->usage_seconds		= p->usage_seconds;
	pi->usage_milliseconds	= p->usage_milliseconds;
	
	if ( p->linear_next == NULL ) return 0;
	return p->linear_next->pid;
}


int info_thread( struct thread *t, struct thread_information *ti )
{
	struct process *p;

	p = t->process;

	memcpy( ti->name, t->name, OS_NAME_LENGTH );
	ti->pid 	= p->pid;
	ti->tid 	= t->tid;
	ti->state 	= t->state;

	ti->left_tid   = 0;
	ti->right_tid  = 0;
	ti->parent_tid = 0;
	ti->child_tid  = 0;

	// tss information
/*
	stack = (uint32*)t->stack;

	ti->tss.eax =  stack[0];
	ti->tss.ebx =  stack[1];
	ti->tss.ecx =  stack[2];
	ti->tss.edx =  stack[3];
	ti->tss.ebp =  stack[4];
	ti->tss.esi =  stack[5];
	ti->tss.edi =  stack[6];
	ti->tss.es  =  stack[7];
	ti->tss.ds  =  stack[8];
	ti->tss.fs  =  stack[9];
	ti->tss.gs  =  stack[10];

	ti->tss.eip       =  stack[11];
	ti->tss.cs        =  stack[12];
	ti->tss.eflags    =  stack[13];

	if ((ti->tss.cs & 0x3) == 0 )
	{
		ti->tss.esp       =  stack[16];
		ti->tss.ss        =  stack[17];
	}
	else
	{
		ti->tss.esp       =  stack[14];
		ti->tss.ss        =  stack[15];
	}
*/

	// linkage information

	if ( t->parent != NULL ) ti->parent_tid = t->parent->tid;
	if ( t->child  != NULL ) ti->child_tid = t->child->tid;
	if ( t->sibling_next != NULL ) 
		ti->right_tid = t->sibling_next->tid;
	if ( t->sibling_prev != NULL ) 
		ti->left_tid = t->sibling_prev->tid;

	if ( t->linear_next == NULL ) return 0;
	return t->linear_next->tid;
}



/// \todo remove

extern int spitter(struct process* p);

extern struct rwlock *process_rwlock;

#include <conio.h>

int spit()
{
	dmesg_xy(0,0, "------- KERNEL INFORMATION --------");
	dmesg_xy(0,1, "PROCESS RWLOCK: (%i,%i,%i)\n",
						process_rwlock->num_readers,
						process_rwlock->num_writers,
						process_rwlock->write_request );

	foreach_process( spitter, READER );
	return 0;
}


