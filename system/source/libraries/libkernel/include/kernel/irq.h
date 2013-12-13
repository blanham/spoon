#ifndef _LIBKERNEL_IRQ_H
#define _LIBKERNEL_IRQ_H

#include <kernel/inttypes.h>

#define IRQ_HANDLED			0
#define IRQ_MISS			-1

typedef int (*irq_func)(int value, void *data);

void __irq_intro();

#ifdef __cplusplus
extern "C" {
#endif


  int smk_request_irq( irq_func fn, int irq_num, const char *name, void *data  );
  int smk_release_irq( int irq_num );

  int smk_ack_irq( int irq_num, int status );
  

#ifdef __cplusplus
}
#endif


   

#endif

