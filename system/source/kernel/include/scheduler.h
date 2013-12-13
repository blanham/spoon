#ifndef _KERNEL_SCHEDULER_H
#define _KERNEL_SCHEDULER_H


struct thread;

			

struct schedule
{
	struct thread *tr;
	uint64_t time; 
};


struct scheduler_info
{
	unsigned int cpu_id;

	struct thread *current_thread;
		
	struct schedule* sched_queue;
	struct schedule* pause_queue;
	volatile unsigned int sched_count;
	volatile unsigned int pause_count;
	volatile unsigned int sched_size;
	volatile unsigned int pause_size;
	volatile unsigned int sched_running;

	volatile unsigned int running;

	volatile unsigned int locked;
	volatile unsigned int lock_scheduler;

	volatile unsigned int position;
};



void load_scheduler( unsigned int cpu_id );

int queue( struct thread *tr );
int dequeue( struct thread *tr );
int pause( struct thread *tr, uint64_t milliseconds );
int restart( struct thread *tr );

#include <apic.h>

static inline void sched_yield()
{
	asm ( "int %0" : : "i" (APIC_INTERRUPT) );
}

#ifndef _KERNEL_CPU_H
#include <cpu.h>
#endif

/// \todo MACROS should not be in lower case

#define current_thread() cpu[ CPUID ].sched.current_thread
#define current_process() cpu[ CPUID ].sched.current_thread->process


#define current_pid() cpu[ CPUID ].sched.current_thread->process->pid
#define current_tid() cpu[ CPUID ].sched.current_thread->tid

/*
static inline struct thread* current_thread()
{
	return cpu[ CPUID ].sched.current_thread;
}

static inline struct process* current_process()
{
	return cpu[ CPUID ].sched.current_thread->process;
}
*/

#endif

