#include <inttypes.h>
#include <apic.h>
#include <misc.h>


#define BOOT_ADDRESS		0x7c000
	
void start_ap_processors()
{
	uint64_t IPI;
	/** Load the __ap_boot over the old boot sector. */

	extern void __ap_boot();
	memcpy( (char*)BOOT_ADDRESS, __ap_boot, 4096 );
	
	
	
	/** Initialize the other APs to start at __ap_boot (see switch.S) */

	IPI = (0xFFull << 56 ) | (3<<18) | (1<<14) | (6<<8) | (0x7c);

	APIC_SET( APIC_interruptCommandHigh, (uint32_t)(IPI>>32) ); 
	APIC_SET( APIC_interruptCommandLow, (uint32_t)(IPI & 0xFFFFFFFF) ); 
	
}



