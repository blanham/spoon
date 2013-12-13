#ifndef _LIBKERNEL_SYSCALLS_H
#define _LIBKERNEL_SYSCALLS_H



/**  \defgroup SYSCALLS  System Call Defines  
 *
 */

/** @{ */

// System Calls

#define		TOTAL_SYSCALLS		0x14

#define     SYS_MEMORY      (0x1<<8)
#define		SYS_SEMAPHORE	(0x2<<8)
#define		SYS_SYSTEM		(0x3<<8)
#define		SYS_PROCESS		(0x4<<8)
#define		SYS_THREAD		(0x5<<8)
#define		SYS_CAP			(0x6<<8)
#define 	SYS_PULSE		(0x7<<8)
#define 	SYS_MESSAGE		(0x8<<8)
#define 	SYS_PORT		(0x9<<8)
#define     SYS_EXEC		(0xA<<8)
#define		SYS_PCI			(0xB<<8)
#define		SYS_TIME		(0xC<<8)
#define		SYS_IRQ			(0xD<<8)
#define 	SYS_IO			(0xE<<8)
#define		SYS_CONSOLE		(0xF<<8)
#define		SYS_LFB			(0x10<<8)
#define 	SYS_INFO		(0x11<<8)
#define     SYS_REBOOT		(0x12<<8)
#define     SYS_SHMEM		(0x13<<8)

// System call functions

#define		SYS_ONE			(1<<0)
#define		SYS_TWO			(1<<1)
#define		SYS_THREE		(1<<2)
#define		SYS_FOUR		(1<<3)
#define		SYS_FIVE		(1<<4)
#define		SYS_SIX			(1<<5)
#define		SYS_SEVEN		(1<<6)
#define		SYS_EIGHT		(1<<7)


/** @} */

#endif

