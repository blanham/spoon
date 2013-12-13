#ifndef _KERNEL_i686_IRQ_H
#define _KERNEL_i686_IRQ_H

#define IRQ_HANDLED		0

#include <threads.h>

void init_irqs();

int  irq_handler( int irq_num );


#endif


