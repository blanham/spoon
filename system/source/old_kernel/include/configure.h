#ifndef _KERNEL_CONFIGURE_H
#define _KERNEL_CONFIGURE_H





									 
#define MAX_PHYSICAL_MEMORY	0x200000

#define MAX_PORTS           1024
#define PULSE_PAGES     	   2
				   
#define PAGE_SIZE		4096

#define app_BASE		0x80000000
#define sK_BASE			0xE0000000
#define sK_CEILING		0xFA000000

#define sK_PAGES_KERNEL		0x20
#define sK_PAGES_PROCESS	0x20
#define sK_PAGES_THREAD		0x20

#define LOCAL_SEM_COUNT         1024
#define GLOBAL_SEM_COUNT        1024


#endif


