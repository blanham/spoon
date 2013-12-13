#ifndef _KERNEL_SYSCALLS_H
#define _KERNEL_SYSCALLS_H


void __system_call();

int syscall_prepare( uint32_t edi, uint32_t esi, uint32_t ebp,
					 uint32_t temp, uint32_t ebx, uint32_t edx,
					 uint32_t ecx, uint32_t eax );

// System call defines

#define		TOTAL_SYSCALLS		0x13

#define     SYS_MEMORY      0x1
#define		SYS_SEMAPHORE	0x2
#define		SYS_SYSTEM		0x3
#define		SYS_PROCESS		0x4
#define		SYS_THREAD		0x5
#define		SYS_CAP			0x6
#define 	SYS_PULSE		0x7
#define 	SYS_MESSAGE		0x8
#define 	SYS_PORT		0x9
#define     SYS_EXEC		0xA
#define		SYS_PCI			0xB
#define		SYS_TIME		0xC
#define		SYS_IRQ			0xD
#define 	SYS_IO			0xE
#define		SYS_CONSOLE		0xF
#define		SYS_LFB			0x10
#define 	SYS_INFO		0x11
#define     SYS_REBOOT		0x12
#define     SYS_SHMEM		0x13


// System call function defines


#define		SYS_ONE			(1<<0)	
#define		SYS_TWO			(1<<1)	
#define		SYS_THREE		(1<<2)		
#define		SYS_FOUR		(1<<3)
#define		SYS_FIVE		(1<<4)
#define		SYS_SIX			(1<<5)
#define		SYS_SEVEN		(1<<6)
#define		SYS_EIGHT		(1<<7)





#endif


