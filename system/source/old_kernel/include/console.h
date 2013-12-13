#ifndef _KERNEL_CONSOLE_H
#define _KERNEL_CONSOLE_H

#include <process.h>

			// This must be >= 1 due to init_consoles
#define		INITIAL_CONSOLES	32

#define		ALL_CONSOLES		0x10000000

struct console
{
   int id;		// The console ID.
   int pages;		// Pages used by the console. (always >= 2)
   void *data;		// pages shared between processes.
   			// The first page is always data area for use
			// by the processes.
   int active_pid;	// The active pid on the console.
};

void init_consoles();

int new_console( int pages );
int stage_console( void *data, int pages );
int delete_console( int id );

int attach_console( struct process *p, int id );
int detach_console( struct process *p );

int set_active_console( int console );

int set_active_pid( int id, int pid );
int get_active_pid( int id );

int console_size( int id );


// --- architecture manipulation

int arch_save_console( void *data, int pages );
int arch_restore_console( void *data, int pages  );
																   
int arch_unmap_console( struct process *proc, void *data, int pages );
int arch_map_console( struct process *proc, void *data, int pages );

#endif


