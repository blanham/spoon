#include <inttypes.h>
#include <dmesg.h>


#include <interrupts.h>
#include <multiboot.h>
#include <misc.h>
#include <gdt.h>
#include <physmem.h>
#include <paging.h>

#include <syscalls.h>

#include <modules.h>

#include <cpu.h>
#include <time.h>
#include <exceptions.h>

#include <irq.h>

#include <apic.h>


#include <smp.h>

#include <process.h>

#include <smk.h>


					

void init( unsigned int magic, multiboot_info_t *mb_info )
{
	zero_bss();

	init_gdt();

	if ( magic != MULTIBOOT_BOOTLOADER_MAGIC )      
	{
		while (1==1) 
			((char*)0xB8000)[0]++; 
	}
	
	dmesg("spoon microkernel\n");

	init_memory( mb_info );

	reserve_modules( mb_info );

	init_paging();

	init_time();

	init_irqs();

	init_cpus();

	init_processes();

	init_smk();

	init_modules();

	start_ap_processors();

	cpu_init( 0 );	// Take a short-cut for the BSP processor.
}









