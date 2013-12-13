#ifndef _KERNEL_SYSENTER_H
#define _KERNEL_SYSENTER_H


#include <inttypes.h>

void sysenter_prepare( uint32_t cs, uint32_t esp, uint32_t eip );
void sysenter_set_esp( uint32_t esp );


#endif

