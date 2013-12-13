#include <apic.h>
#include <cpu.h>
#include <time.h>
#include <dmesg.h>


#define TEST_DURATION		200


void load_apic( unsigned int cpu_id )
{
	uint32_t after;

	// Software enable the local APIC. Some BIOS's appear to disable it.
	after = APIC_GET( APIC_spurious );
	APIC_SET( APIC_spurious,  (after | (1<<8)) );

	APIC_SET( APIC_LVTtimer, APIC_INTERRUPT );
	APIC_SET( APIC_TIMERdivider, APIC_TIMERdivideBy128 ); 
	
	
	// Starts it counting down..
	APIC_SET( APIC_TIMERinitialCount, 0xFFFFFFFF );
	delay( TEST_DURATION );	// Delay a few ms
	APIC_SET( APIC_LVTtimer, 0); 
	after = 0xFFFFFFFF - APIC_GET( APIC_TIMERcurrentCount );

	cpu[ cpu_id ].busSpeed = (after * (1000 / TEST_DURATION)) / 128;

	APIC_SET( APIC_TIMERdivider, APIC_TIMERdivideBy16 );

	dmesg("CPU %i bus frequency = %i Hz\n", cpu_id, cpu[ cpu_id ].busSpeed );
}


void set_apic_distance( unsigned int cpu_id, unsigned int milliseconds )
{
	/// \todo Find a way to precalculate...
	uint32_t after = ((cpu[ cpu_id ].busSpeed * 16) / 1000) * milliseconds;
	
	//Set the new local APIC timer divider
	APIC_SET( APIC_LVTtimer, APIC_INTERRUPT );
	APIC_SET( APIC_TIMERinitialCount, (uint32_t)after);
}



