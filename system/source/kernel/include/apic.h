#ifndef _KERNEL_APIC_H
#define _KERNEL_APIC_H

#include <inttypes.h>


#define APIC_apicID 		        	0xFEE00020ul
#define APIC_version        		 	0xFEE00030ul
#define APIC_taskPriority      			0xFEE00080ul
#define APIC_endOfInterrupt      		0xFEE000B0ul
#define APIC_logicalDestination      	0xFEE000D0ul
#define APIC_destinationFormat      	0xFEE000E0ul
#define APIC_spurious         			0xFEE000F0ul
#define APIC_interruptCommandLow   		0xFEE00300ul
#define APIC_interruptCommandHigh   	0xFEE00310ul
#define APIC_LVTtimer         			0xFEE00320ul
#define APIC_LVTperfCounter      		0xFEE00340ul
#define APIC_LVT_LINT0         			0xFEE00350ul
#define APIC_LVT_LINT1         			0xFEE00360ul
#define APIC_LVTerror         			0xFEE00370ul
#define APIC_TIMERinitialCount      	0xFEE00380ul
#define APIC_TIMERcurrentCount      	0xFEE00390ul
#define APIC_TIMERdivider      			0xFEE003E0ul

#define APIC_TIMERdivideBy2        		0x00000000
#define APIC_TIMERdivideBy4        		0x00000008
#define APIC_TIMERdivideBy8        		0x00000002
#define APIC_TIMERdivideBy16       		0x0000000A
#define APIC_TIMERdivideBy32       		0x00000001
#define APIC_TIMERdivideBy64       		0x00000009
#define APIC_TIMERdivideBy128      		0x00000003
#define APIC_TIMERdivideBy1        		0x0000000B

#define APIC_TIMERperiodic          	0x00020000


#define APIC_INTERRUPT		0x50

extern uint32_t busSpeed;

void load_apic( unsigned int cpu_id );

void set_apic_distance( unsigned int cpu_id, unsigned int milliseconds );

#define APIC_SET( reg, value ) *((uint32_t*)reg) = value
#define APIC_GET( reg ) *((uint32_t*)reg)

#endif

