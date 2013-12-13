#ifndef _LIBKERNEL_THREADS_H
#define _LIBKERNEL_THREADS_H	1

#include <types.h>
#include <kernel/lengths.h>


#define NO_PRIORITY			0
#define LOW_PRIORITY		1
#define NORMAL_PRIORITY		5
#define HIGH_PRIORITY		20
#define RIDICULOUS_PRIORITY	100
#define REALTIME_PRIORITY	255


typedef int32 (*thread_func)(void *data);
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




int32 smk_spawn_thread( thread_func  func, const char *name, int32 priority, void* data );

int32 smk_set_thread_state( int32 tid, uint32 state );

int32 smk_suspend_thread( int32 tid );
int32 smk_resume_thread( int32 tid );
int32 smk_kill_thread( int32 tid );


// --------------------

int32 smk_find_thread_id( const char name[OS_NAME_LENGTH] );
int32 smk_find_thread_name( int32 tid, const char *name );

int32 smk_gettid();


// ------ TLS support -----------------------

int smk_set_tls_location( void* location );
int smk_get_tls_location( void** location );
int smk_set_tls_value( int tid, void* value );
int smk_get_tls_value( int tid, void** value );

#ifdef __cplusplus
}       
#endif

#endif

