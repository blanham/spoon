#ifndef _KERNEL_FPU_H
#define _KERNEL_FPU_H



#define FPU_MP		(1<<1)
#define FPU_EM		(1<<2)
#define FPU_TS		(1<<3)
#define FPU_NE		(1<<5)
#define FPU_CLEAN	(0xFFFFFFFF - FPU_MP - FPU_EM - FPU_TS - FPU_NE)


struct thread;	// declare


void load_fpu( unsigned int cpu_id );


void clean_fpu( struct thread *t );
void save_fpu( struct thread *t );
void restore_fpu( struct thread *t );


static inline void set_fpu_trap()
{
	asm	(    "mov %%cr0, %%eax\n"
			 "and  %0, %%eax\n"
		     "or   %1, %%eax\n"
		     "mov %%eax, %%cr0\n"
	 :
	 : "g" (FPU_CLEAN), "g" (FPU_TS | FPU_MP)
	 : "eax"
	);
}

static inline void release_fpu_trap()
{
	asm	( "clts" );
}




#endif

