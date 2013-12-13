#ifndef _KERNEL_i686_MISC_H
#define _KERNEL_i686_MISC_H

#include <types.h>

void zero_bss();

int32 invlpg( uint32 eax );
uint32 cr0();
uint32 cr2();
uint32 cr3();
uint32 cr4();

void ltr( uint32 gdt_entry );
void call_task( uint32 gdt );
void jump_task( uint32 gdt );

void set_cpu_flags( uint32 flags );
uint32 cpu_flags();






#endif

