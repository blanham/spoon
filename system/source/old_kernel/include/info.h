#ifndef _KERNEL_INFO_H
#define _KERNEL_INFO_H

#include <types.h>
#include <process.h>
#include <threads.h>

struct system_information
{
	char kernel_name[50];
	uint32 kernel_version;
	uint32 cpu_speed;
	uint32 memory_total;
	uint32 memory_free;
	uint32 uptime;
	uint32 sched_length;
	uint32 sched_running;
	uint32 process_length;
	uint32 thread_length;
};


struct process_information
{
	char name[32];
	int32 pid;
	int32 threads;
	uint32 state;
	uint32 usage_cpu;
	uint32 usage_kmem;
	uint32 usage_umem;
	uint32 usage_seconds;
	uint32 usage_milliseconds;
};

struct thread_information 
{
	char name[32];
	int32 pid;
	int32 tid;
	uint32 state;
	int32 parent_tid;
	int32 left_tid;
	int32 right_tid;
	uint32 child_tid;
};





int info_system( struct system_information *si );
int info_process( struct process*, struct process_information *pi );
int info_thread( struct thread*, struct thread_information *ti );


int spit();

#endif

