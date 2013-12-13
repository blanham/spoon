#include <configure.h>
#include <types.h>


#include "io.h"



extern char __bss_start;
extern char __bss_end;


void zero_bss()
  {
	  char *bss;
	  
	  
	  for ( bss = &__bss_start; bss < &__bss_end; bss++ )
		*bss = 0;
  }

int invlpg( uint32 eax )
{
	__asm__
		(
		 "\n"
		   "mov %0, %%eax;\n"
		   "invlpg (%%eax);\n"
		 
		 : 
		 : "g" (eax)
		 : "eax"
		);
	return 0;
}




uint32 cr0()
{
	uint32 p;
        __asm__ (   " mov %%cr0,%%eax;\n"
					" mov %%eax, %0;\n"
		: "=g" (p)
		:
		: "eax" );
	return p;
}

uint32 cr2()
{
    uint32 address;
    __asm__
      (
        "mov %%cr2, %%eax;\n"
		"mov %%eax, %0;\n"
      
      : "=g" (address)
      :
      : "eax"
      );
    return address;
}


uint32 cr3()
{
    uint32 address;
    __asm__
      (
        "mov %%cr3, %%eax;\n"
		"mov %%eax, %0;\n"
      
      : "=g" (address)
      :
      : "eax"
      );
    return address;
}

uint32 cr4()
{
    uint32 address;
    __asm__
      (
        "mov %%cr4, %%eax;\n"
		"mov %%eax, %0;\n"
      
      : "=g" (address)
      :
      : "eax"
      );
    return address;
}





void ltr( uint32 gdt_entry )
{
   __asm__ ( " ltr %0 " : : "g" (gdt_entry));
}






uint32 cpu_flags()
{
  uint32 old_flags;
    __asm__
      ( 
        "\n"
		  "pushfl\n"
		  "pop %%eax\n"
		  "mov %%eax, %0\n"
	
	: "=g" (old_flags)
	:
	: "eax"
      );
      
  return old_flags;
}

void set_cpu_flags( uint32 flags )
{
    __asm__
      ( 
        "\n"
		  "mov %0, %%eax\n"
		  "pushl %%eax\n"
		  "popfl\n"
	
	: 
	: "g" (flags)
	: "eax"
      );
}

void call_task( uint32 gdt )
{
   uint32 sel[2];

      sel[0] = 0xDEAD;
      sel[1] = gdt;
     __asm__ __volatile__ ( "lcall *(%0)": :"g" ((uint32*)&sel) ) ;
}

void jump_task( uint32 gdt )
{
   uint32 sel[2];

      sel[0] = 0xDEAD;
      sel[1] = gdt;
      __asm__ __volatile__ ("ljmp *(%0)": :"g" ((uint32*)&sel) );
}




// ------------------------------------------------------------

int arch_yield() 
{ 
  __asm__ ("int $0x30"); 
  return 0;
}


int halt_cpu()
{
	__asm__ ( "hlt" );
	return 0;
}

void arch_reboot()
{
    int32 i, j;

    *((uint16*)0x472) = 0x1234;
	
    while (1==1)
		for (i = 0 ; i < 100 ; ++i )
		{
		    for(j = 0; j < 100000 ; ++j )  
					outb(0x64, 0xfe);	 /* pulse reset low */
		}
}


