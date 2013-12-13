#ifndef _KERNEL_CPU_H
#define _KERNEL_CPU_H

#include <inttypes.h>
#include <gdt.h>
#include <apic.h>
#include <scheduler.h>
#include <stats.h>

#ifndef MAX_CPU
#define MAX_CPU		8
#endif

#define CPUID  (APIC_GET( APIC_apicID ) >> 24)


struct cpu_info
{
	int present;

	int gdt_boot;
	int gdt_system;

	struct TSS* system_tss;

	uint32_t boot_stack;		// See switch.S and cpu.c
	uint32_t scheduler_stack;

	struct scheduler_info sched;

	unsigned int bogomips;
	unsigned int busSpeed;

	struct IDT_GATE *idt;
	
	struct TSS* exception_tss[32];
	segment_descriptor *gdt;
	volatile unsigned int gdt_lock;


	struct s_time st_systemTime;
	struct s_time st_schedulerTime;
	
};


extern struct 		cpu_info cpu[ MAX_CPU ];
extern uint32_t		boot_stack[4096];
extern volatile int	cpu_count;


void init_cpus();
void cpu_init( uint32_t stack );

#endif

