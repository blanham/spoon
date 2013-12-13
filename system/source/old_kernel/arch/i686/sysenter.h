#ifndef _KERNEL_i686_SYSENTER_H
#define _KERNEL_i686_SYSENTER_H


#include <types.h>

void sysenter_prepare( uint32 cs, uint32 esp, uint32 eip );
void sysenter_set_esp(  uint32 esp );


#endif

