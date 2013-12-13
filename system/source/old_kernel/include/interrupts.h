#ifndef _KERNEL_INTERRUPTS_H
#define _KERNEL_INTERRUPTS_H

void enable_interrupts();
int disable_interrupts();

extern unsigned int irq_disableDepth;

/** Start a critical region */
static inline void BEGIN_CRITICAL_REGION()
{
	disable_interrupts();
	irq_disableDepth += 1;
}

/** End a critical region */
static inline void END_CRITICAL_REGION()
{
	irq_disableDepth -= 1;
	if ( irq_disableDepth == 0 )
			enable_interrupts();
}



#endif

