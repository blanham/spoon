#ifndef _LIBKERNEL_SETJMP_H
#define _LIBKERNEL_SETJMP_H

#include <types.h>


typedef struct 
{
	uint32 regs[32];
} smk_jmpbuf[1];





#ifdef __cplusplus
extern "C" {
#endif


int   smk_setjmp( smk_jmpbuf envp );
void  smk_longjmp( smk_jmpbuf envp, int value);


#ifdef __cplusplus
}
#endif



#endif

