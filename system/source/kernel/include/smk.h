#ifndef _KERNEL_SMK_H
#define _KERNEL_SMK_H

#include <process.h>
#include <threads.h>
#include <cpu.h>

extern struct process *smk_process;
extern struct thread  *smk_idle[MAX_CPU];


void init_smk();

void load_smk( unsigned int cpu_id );

#endif

