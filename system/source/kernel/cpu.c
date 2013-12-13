#include <assert.h>
#include <inttypes.h>
#include <cpu.h>
#include <time.h>
#include <modules.h>
#include <fpu.h>
#include <pci.h>


/** The boot stack is a uint32_t array that the BSP processor
 * will use in jump.S as the initial stack.
 */
uint32_t	boot_stack[4096] __attribute__ ((aligned (4096)));

/** The ap_stack is another stack that the AP processors will
 * use, in sequence, when booting up until they acquire their
 * own stack.
 */
uint32_t	ap_stack[4096] __attribute__ ((aligned (4096)));


struct 		cpu_info cpu[ MAX_CPU ];

volatile int 		cpu_count	=  0;		// Incremented by each CPU.
volatile unsigned int cpu_lock	=  0;		// For cpu_count 


void init_cpus()
{
	int i;

	assert( cpu_lock == 0 );
	assert( cpu_count == 0 );

	for ( i = 0; i < MAX_CPU; i++ )
	{
		cpu[i].present = 0;

		cpu[i].st_systemTime.ops   		= 0;
		cpu[i].st_systemTime.last_rdtsc	= 0;
		cpu[i].st_systemTime.usage 		= 0;

		cpu[i].st_schedulerTime.ops   		= 0;
		cpu[i].st_schedulerTime.last_rdtsc  = 0;
		cpu[i].st_schedulerTime.usage 		= 0;
	}
}



#include <atomic.h>
#include <gdt.h>
#include <paging.h>
#include <interrupts.h>
#include <apic.h>
#include <syscalls.h>
#include <sysenter.h>
#include <misc.h>
#include <exceptions.h>
#include <smk.h>
#include <scheduler.h>

#include <irq.h>


void cpu_init( unsigned int stack )
{
	unsigned int cpu_id; 
	unsigned int i = 0;

	// Find the next CPU ID available
	acquire_spinlock( &(cpu_lock) );

		cpu_id = cpu_count++;
		APIC_SET( APIC_apicID, (cpu_id<<24) ); 

	release_spinlock( &(cpu_lock) );

	cpu[ cpu_id ].present = 1;
	cpu[ cpu_id ].boot_stack = stack;

	load_time( cpu_id );
	
		// The BSP (well, #0) is in charge of taking count.
		if ( cpu_id == 0 )
		{
			for ( i = 0; i < 10; i++ )
			{
				if ( cpu_count != 1 ) 
				{
					// If this case occurs, an AP processor has incremented
					// cpu_count. The others can't be too far behind.
					delay(500);
					break;
				}

				delay(50);
			}

			dmesg("Detected %i CPU's.\n", cpu_count );
		}
			
	
	// Safe to use cpu_count now.
	

	load_gdt( cpu_id );
	
	load_paging( cpu_id );

	load_interrupts( cpu_id );

	load_irqs( cpu_id );

	load_exceptions( cpu_id );

	load_fpu( cpu_id );

	load_apic( cpu_id );

	load_smk( cpu_id );

	load_scheduler( cpu_id );

	// If this is the first CPU, start the first module's main thread.
	if ( cpu_id == 0 ) 
	{
		init_pci();				/// \todo Determine why it crashes in init()
		start_first_module();	
	}

	sysenter_prepare( GDT_SYSTEMCODE, 0,  (uint32_t)__system_call );

	// Do the final jump into the scheduling routine
	call_task( cpu[ cpu_id ].gdt_system );
}




