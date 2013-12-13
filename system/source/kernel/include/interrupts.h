#ifndef _KERNEL_INTERRUPTS_H
#define _KERNEL_INTERRUPTS_H

#include <inttypes.h>

#define IDT_PRESENT  0x8000      //  10000000 00000000b
#define IDT_TRAP     0x0700      //  00000111 00000000b
#define IDT_INT      0x0600      //  00000110 00000000b
#define IDT_TASK     0x0500      //  00000101 00000000b
#define IDT_32       0x0800      //  00001000 00000000b

#define IDT_DPL0     0x0000      //  00000000 00000000b
#define IDT_DPL1     0x2000      //  00100000 00000000b
#define IDT_DPL2     0x4000      //  01000000 00000000b
#define IDT_DPL3     0x6000      //  01100000 00000000b


struct IDT_GATE
{
  unsigned int offset15_0 	: 16;
  unsigned int segment		: 16;
  unsigned int flags 		: 16;
  unsigned int offset16_31	: 16;
} __attribute__ ((packed));


void load_interrupts( unsigned int cpu_id );

void set_interrupt( unsigned int cpu_id, 
					int num, unsigned int type, 
					unsigned int segment, 
					uint32_t offset, 
					unsigned int level);


void enable_interrupts();
void disable_interrupts();



#endif

