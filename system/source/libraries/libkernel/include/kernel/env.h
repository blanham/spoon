#ifndef _KERNEL_ENV_H
#define _KERNEL_ENV_H


#ifdef __cplusplus
extern "C" {
#endif


int smk_setenv( const char *name, void *data, int size );
int smk_getenv( const char *name, void *data, int size );

int smk_getenv_size( const char *name );
int smk_getenv_info( int i, const char *name, int *size );

int smk_clearenv( const char *name );


#ifdef __cplusplus
}
#endif

#endif

