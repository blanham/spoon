#ifndef _KERNEL_FPU_H
#define _KERNEL_FPU_H



#define FPU_MP		(1<<1)
#define FPU_EM		(1<<2)
#define FPU_TS		(1<<3)
#define FPU_NE		(1<<5)
#define FPU_CLEAN	(0xFFFFFFFF - FPU_MP - FPU_EM - FPU_TS - FPU_NE)


#include <threads.h>

void init_fpu();

void set_fpu_trap();
void clean_fpu( struct thread *t );
void save_fpu( struct thread *t );
void restore_fpu( struct thread *t );
void release_fpu_trap();


#endif

