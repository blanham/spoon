#ifndef _KERNEL_ASSERT_H
#define _KERNEL_ASSERT_H

#include <dmesg.h>
#include <interrupts.h>

#ifndef NDEBUG

#define assert(condition)			\
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

