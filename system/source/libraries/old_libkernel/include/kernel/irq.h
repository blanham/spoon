#ifndef _LIBKERNEL_IRQ_H
#define _LIBKERNEL_IRQ_H

#include <types.h>


typedef int (*irq_func)(int value, void *data);

void __irq_intro();

#ifdef __cplusplus
extern "C" {
#endif


  int smk_request_irq( irq_func fn, int irq_num, const char *name, void *data  );
  int smk_release_irq( int irq_num );
  

#ifdef __cplusplus
}
#endif


   

#endif

