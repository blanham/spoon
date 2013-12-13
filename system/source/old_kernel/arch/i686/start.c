#include <types.h>
#include <dmesg.h>
#include <conio.h>
#include <interrupts.h>

#include "gdt.h"
#include "misc.h"
#include "int.h"
#include "memory.h"
#include "time.h"
#include "paging.h"
#include "exceptions.h"
#include "lfb.h"
#include "pci.h"
#include "cpu.h"
#include "fpu.h"
#include "syscalls.h"
#include "modules.h"


extern int kernel(char *cmdline);


#include <params.h>

void arch_init( uint32 multiboot_magic_number, multiboot_info_t *multiboot_info)
{
  disable_interrupts();
  
  zero_bss();

  gdt_init(); 

  init_lfb( multiboot_info );
  
  clear();

  parse_command_line( (char*)multiboot_info->cmdline  );

  if ( multiboot_magic_number != MULTIBOOT_BOOTLOADER_MAGIC )      
  {
	dmesg("%!%s",  "please load the spoon microkernel with a ");
	dmesg("%!%s\n","multiboot compliant boot loader.");
	dmesg("%!%s\n","GRUB is a good example of one. http://gnu.org/");
    while (1==1) {}
  }

  arch_init_memory( multiboot_info );    

  reserve_modules( multiboot_info );
  
  init_cpu();

  init_fpu();
  
  init_paging();                      
  
  init_time();

  init_interrupts();               
 
  init_exceptions();

  init_pci();

  init_syscalls();

  kernel( (char*)multiboot_info->cmdline );
}


