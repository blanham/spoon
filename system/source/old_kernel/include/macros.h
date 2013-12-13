#include <memory.h>
#include <interrupts.h>
#include <dmesg.h>
#include <configure.h>

#include <params.h>

#ifndef _MACROS_H
#define _MACROS_H



#ifndef NDEBUG

#define ASSERT(condition)			\
		if ( ! (condition) )		\
		{							\
			disable_interrupts();	\
			dmesg("%!%s(%i): %s(%s)\n", __FILE__, __LINE__, "assert condition failed", #condition );	\
			dmesg("%!%s\n","kernel halted.");		\
			while (1==1) {}		\
		}
#else
#define ASSERT(condition)	
#endif


#endif

