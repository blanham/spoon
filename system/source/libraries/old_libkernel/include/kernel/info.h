#ifndef _LIBKERNEL_INFO_H
#define _LIBKERNEL_INFO_H

#include <types.h>
#include <kernel/tss.h>

#ifdef __cplusplus
extern "C" {
#endif



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
	struct TSS tss;
};




int32 smk_info_mem_free();
int32 smk_info_mem_total();

int32 smk_info_system( struct system_information *si );
int32 smk_info_process( int32 pid, 
                  struct process_information *pi );
int32 smk_info_thread( int32 pid, int32 tid, 
		  struct thread_information *ti );

int smk_spit();

#ifdef __cplusplus
}
#endif



#endif

