#ifndef _LIBKERNEL_SEM_H
#define _LIBKERNEL_SEM_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif



int smk_acquire_sem(sem_id sem);
int smk_acquire_sem_etc(sem_id sem, 
		    uint32 count, 
		    uint32 flags, 
		    bigtime_t timeout);
		  

int smk_create_sem(uint32 thread_count, const char *name);
int smk_delete_sem(sem_id sem);

int smk_release_sem(sem_id sem);

// global 

int smk_acquire_global_sem(sem_id sem);
int smk_acquire_global_sem_etc(sem_id sem, 
		    uint32 count, 
		    uint32 flags, 
		    bigtime_t timeout);
		  

int smk_create_global_sem(uint32 thread_count, const char *name);
int smk_delete_global_sem(sem_id sem);

int smk_release_global_sem(sem_id sem);




#ifdef __cplusplus
}
#endif



#endif

