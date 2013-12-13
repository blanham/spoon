#include <types.h>
#include <macros.h>
#include <dmesg.h>
#include <atomic.h>
#include <configure.h>
#include <alloc.h>
#include <conio.h>
#include <process.h>
#include <ioperms.h>


// -----------------------

static int *ioperm_table;

static spinlock ioperm_lock = INIT_SPINLOCK;


int arch_io_grant( struct process *proc, int ioport );
int arch_io_revoke( struct process *proc, int ioport );
int arch_io_grant_priv( struct process *proc );
int arch_io_revoke_priv( struct process *proc );


// -----------------------

int init_ioperms()
{
	int size;

	dmesg( "initializing the io permissions table.\n" );
	
	size = (TOTAL_IOPERMS * sizeof(int));
	ioperm_table = (int*) malloc( size );
	
	for ( size = 0; size < TOTAL_IOPERMS; size++ )
			ioperm_table[size] = -1;

	return 0;
}

// -------------------------------------

int io_owner( int ioport )
{
	int ans;
	
	if ( ioport < 0 ) return -1;
	if ( ioport >= TOTAL_IOPERMS ) return -1;

	acquire_spinlock( &ioperm_lock );
		ans = ioperm_table[ioport];
	release_spinlock( &ioperm_lock );
	
	return ans; 
}

int io_grant( struct process *proc, int ioport )
{
	if ( ioport < 0 ) return -1;
	if ( ioport >= TOTAL_IOPERMS ) return -1;
	if ( io_owner( ioport ) != -1 ) return -1;

	if ( arch_io_grant( proc, ioport ) != 0 ) return -1;
	
	acquire_spinlock( &ioperm_lock );
		ioperm_table[ioport] = proc->pid;
	release_spinlock( &ioperm_lock );
	
	return 0;
}

int io_revoke( struct process *proc, int ioport )
{
	if ( ioport < 0 ) return -1;
	if ( ioport >= TOTAL_IOPERMS ) return -1;

	if ( arch_io_revoke( proc, ioport ) != 0 ) return -1;
	
	acquire_spinlock( &ioperm_lock );
		ioperm_table[ioport] = -1;
	release_spinlock( &ioperm_lock );
	
	return 0;
}


int io_grant_priv( struct process *proc )
{
	if ( arch_io_grant_priv( proc ) != 0 ) return -1;
	proc->flags = proc->flags | IOPRIV;
	return 0;
}

int io_revoke_priv( struct process *proc )
{
	if ( arch_io_revoke_priv( proc ) != 0 ) return -1;
	proc->flags = proc->flags & ~(IOPRIV);
	return 0;
}


