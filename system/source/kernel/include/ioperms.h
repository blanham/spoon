#ifndef _KERNEL_IOPERMS_H
#define _KERNEL_IOPERMS_H


#define		IOPRIV		(1<<0)


int io_grant_priv( struct process *proc );
int io_revoke_priv( struct process *proc );

#endif

