#ifndef _KERNEL_LFB_H
#define _KERNEL_LFB_H

#include <types.h>
#include <process.h>


void set_lfb( uint32 width, 
	      uint32 height, 
	      uint32 depth, 
	      uint32 type, 
	      void *ptr );

int get_lfb( uint32 *width,
	     uint32 *height,
	     uint32 *depth,
	     uint32 *type,
	     void **ptr );


uint32 provide_lfb(  struct process *proc );
uint32 release_lfb(  struct process *proc );


int using_lfb();



#endif

