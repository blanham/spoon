#include <inttypes.h>
#include <interrupts.h>
#include <gdt.h>
#include <dmesg.h>

#include <physmem.h>
#include <cpu.h>

#include <apic.h>

void  disable_interrupts()
{
    asm ( "cli" );
}

void  enable_interrupts()
{
    asm (" sti ");
}



void load_interrupts( unsigned int cpu_id )
{
  int i;
  uint32_t ses[2];  

  // Allocate the interrupt table.
  	cpu[ cpu_id ].idt = 
			(struct IDT_GATE*)
				memory_alloc( sizeof(struct IDT_GATE) * 256 / 4096 + 1 );
  
  extern void __null_int();

  for (i = 0; i < 256; i++)
    set_interrupt( cpu_id, i, IDT_PRESENT | IDT_32 | IDT_INT, 
					GDT_SYSTEMCODE, (uint32_t)__null_int, IDT_DPL3 );

  extern void __soft_scheduler();
	
    set_interrupt( cpu_id, APIC_INTERRUPT, IDT_PRESENT | IDT_32 | IDT_INT, 
					GDT_SYSTEMCODE, (uint32_t)__soft_scheduler, IDT_DPL3 );
  

	
  // load the interrupt table
    ses[0] = (256*8) << 16;
    ses[1] = (uint32_t)( cpu[cpu_id].idt );
    asm ("lidt (%0)": :"g" (((char *) ses)+2));

	dmesg( "CPU %i loaded interrupts\n", cpu_id );
}


void set_interrupt( unsigned int cpu_id, 
					int num, unsigned int type, 
					unsigned int segment, 
					uint32_t offset, 
					unsigned int level)
{
  cpu[ cpu_id ].idt[num].segment =  segment;
  cpu[ cpu_id ].idt[num].flags = level | type;
  cpu[ cpu_id ].idt[num].offset15_0  = offset & 0xFFFF;
  cpu[ cpu_id ].idt[num].offset16_31 = (offset >> 16) & 0xFFFF;
}


