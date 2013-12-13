#include <process.h>
#include <ioperms.h>



int io_grant_priv( struct process *proc )
{
	proc->flags = proc->flags | IOPRIV;
	return 0;
}

int io_revoke_priv( struct process *proc )
{
	proc->flags = proc->flags & ~(IOPRIV);
	return 0;
}


