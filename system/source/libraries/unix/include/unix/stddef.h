#ifndef _STDDEF_H
#define _STDDEF_H

#ifndef	NULL
#define NULL		0
#endif


#ifndef _HAVE_PTRDIFF_T
#define _HAVE_PTRDIFF_T
typedef signed long int	ptrdiff_t;
#endif

#ifndef _HAVE_SIZE_T
#define _HAVE_SIZE_T
typedef	unsigned int	size_t;
#endif


#if ( (! defined _HAVE_WCHAR_T) && (! defined __cplusplus ) )
#define _HAVE_WCHAR_T
typedef int	wchar_t;
#endif


#define offsetof(type, memberdesig)	\
			((const size_t)((ptrdiff_t)&(type.memberdesig) - (ptrdiff_t)&type))


#endif

