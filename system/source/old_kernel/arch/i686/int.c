#include <types.h>
#include <configure.h>
#include <dmesg.h>
#include <macros.h>
#include <conio.h>

#include "int.h"
#include "gdt.h"
#include "eflags.h"



static struct IDT_GATE idt[256];  // interrupt table
                                  // As you can see, it's kept inside the kernel.

void init_interrupts()
{
  int32 i;
  uint32 ses[2];  

  dmesg("initializing interrupts.\n");
  dmesg("building interrupt table.\n"); 


   // set all interrupts to point to my __null_int which just returns to the calling
   // program with a not-implemented return code.
  for (i = 0; i < 256; i++)
    set_interrupt(  i, IDT_PRESENT | IDT_32 | IDT_INT, 
					system_CS(), (uint32)__null_int, IDT_DPL3 );


  // load the interrupt table
    ses[0] = (256*8) << 16;
    ses[1] = (uint32)&idt;
   __asm__ __volatile__ ("lidt (%0)": :"g" (((char *) ses)+2));

   
  dmesg("loaded interrupt table.\n"); 

}

void set_interrupt( uint16 int_num, uint16 type, uint16 segment, uint32 offset, uint16 level)
{
  idt[int_num].segment =  segment;
  idt[int_num].flags = level | type;
  idt[int_num].offset15_0  = offset & 0xFFFF;
  idt[int_num].offset16_31 = (offset >> 16) & 0xFFFF;
}

//--------------------------------


/** Disables interrupts on the CPU. If interrupts were
 * enabled, the function returns 1. If not, it will return
 * 0.
 */
int  disable_interrupts()
{
	uint32 cpu_flags;
		
    __asm__ __volatile__ 
			(  " pushfl\n"
			   " pop %%eax\n"
			   " mov %%eax, %0\n" 
			   " cli\n" 
			  : "=g" (cpu_flags)
			  :
			  : "eax"
			); 

	if ( (cpu_flags & EFLAG_INTERRUPT) != 0 ) 
			return 1;

	return 0;
}

/** Just enables interrupts. */
void  enable_interrupts()
{
    __asm__ (" sti ");
}


