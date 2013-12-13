#ifndef _LIBKERNEL_MISC_H
#define _LIBKERNEL_MISC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>


  void smk_disable_interrupts();
  void smk_enable_interrupts();


  void smk_exit(int code);
  int smk_go_dormant();
  int smk_go_dormant_t( int milliseconds );

  void smk_reboot();

#ifdef __cplusplus
}       
#endif
#endif

