#ifndef _KERNEL_VARARGS_H
#define _KERNEL_VARARGS_H	

#include <inttypes.h>

#ifndef _HAVE_VA_LIST
#define _HAVE_VA_LIST
typedef struct 
{
	void **start;
	void **ptr;
} va_list;
#endif


#define		va_start( ap, lastarg )					\
			ap.ptr = (void**)( (uintptr_t)&lastarg + sizeof(const char*) );	\
			ap.start = ap.ptr;	


#define		va_arg( ap, type) \
			((type)*(ap.ptr)); \
			ap.ptr = (void**)( ((uintptr_t)ap.ptr) + sizeof( type ) );
		
#define		va_end( ap )	\
			ap.ptr = NULL;

#endif

