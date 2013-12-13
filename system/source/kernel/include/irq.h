#ifndef _KERNEL_IRQ_H
#define _KERNEL_IRQ_H


struct thread;


void init_irqs();
void load_irqs( unsigned int cpu_id );


int request_irq( struct thread *t, int irq );
int release_irq( struct thread *t, int irq );
int irq_ack( struct thread *t, int irq, int status );

#endif


