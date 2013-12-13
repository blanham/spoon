#include <process.h>
#include <ioperms.h>

#include "eflags.h"
#include "thread_data.h"
#include "misc.h"

int arch_io_grant( struct process *proc, int ioport )
{
	return io_grant_priv( proc );
}

int arch_io_revoke( struct process *proc, int ioport )
{
	//return io_release_priv( pid );
	return 0;
}


int arch_io_grant_priv( struct process *proc )
{
	return 0;
}

int arch_io_revoke_priv( struct process *proc )
{
	return 0;
}



