#ifndef _KERNEL_IRQ_H
#define _KERNEL_IRQ_H


#define IRQ_HANDLED		0

#include "threads.h"
#include "lengths.h"

struct irq_handler
{
	char name[OS_NAME_LENGTH];
	int pid;
	int tid;
	int masked;
	struct thread *t;
};


void init_irqs();

void mask_irq(int);
void unmask_irq(int);
int  masked(int);

int  irq_handler( int irq_num);

int32 request_irq( struct thread *t, char n[OS_NAME_LENGTH], int irq );
int32 release_irq( struct thread *t, int irq );




int arch_init_irqs();
int arch_mask_irq( int );
int arch_unmask_irq( int );
int arch_ack_irq( int );




#endif






