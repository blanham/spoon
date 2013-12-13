#ifndef _KERNEL_SMK_GC_H
#define _KERNEL_SMK_GC_H


#include "threads.h"


extern struct thread* smk_gc;


int init_smk_gc();

int signal_gc( int pid, int tid );

int gc_needed();
int gc_check();


#endif

