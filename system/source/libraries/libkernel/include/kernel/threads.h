#ifndef _LIBKERNEL_THREADS_H
#define _LIBKERNEL_THREADS_H

#include <kernel/inttypes.h>


#define NO_PRIORITY			0
#define LOW_PRIORITY		1
#define NORMAL_PRIORITY		5
#define HIGH_PRIORITY		20
#define RIDICULOUS_PRIORITY	100
#define REALTIME_PRIORITY	255


typedef int (*thread_func)(void *data);

#ifdef __cplusplus
extern "C" {
#endif

#define THREAD_SYSTEM			0
#define THREAD_RUNNING			1
#define THREAD_SUSPENDED		2
#define THREAD_DORMANT			3
#define THREAD_STOPPED			4
#define THREAD_SEMAPHORE		5
#define THREAD_SLEEPING			6
#define THREAD_WAITING			7
#define THREAD_FINISHED			8
#define THREAD_CRASHED			9
#define THREAD_IRQ				10




int smk_spawn_thread( thread_func  func, 
						const char *name, 
						int priority, 
						void* data );

int smk_set_thread_state( int tid, unsigned int state );

int smk_suspend_thread( int tid );
int smk_resume_thread( int tid );
int smk_kill_thread( int tid );

/*

// --------------------

int smk_find_thread_id( const char *name );
int smk_find_thread_name( int tid, const char *name );
*/

int smk_gettid();


// ------ TLS support -----------------------

/*
int smk_set_tls_location( void* location );
int smk_get_tls_location( void** location );
int smk_set_tls_value( int tid, void* value );
int smk_get_tls_value( int tid, void** value );
*/

#ifdef __cplusplus
}       
#endif

#endif

