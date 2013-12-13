#ifndef _LIBKERNEL_TIME_H
#define _LIBKERNEL_TIME_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

  uint32 smk_cmos_time();

  int32 smk_system_time( uint32 *seconds, uint32 *milliseconds ); 
  
  void smk_delay( uint32 milliseconds );
  void smk_ndelay( uint32 milliseconds );
  void smk_release_timeslice();
  void smk_sleep( uint32 seconds );

  int smk_wait_process( int32 pid, int32 *rc );
  int smk_wait_thread( int32 tid, int32 *rc );
  
#ifdef __cplusplus
}       
#endif


#endif
