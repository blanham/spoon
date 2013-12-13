#ifndef _KERNEL_IOPERMS_H
#define _KERNEL_IOPERMS_H

#define TOTAL_IOPERMS	(65536)



int init_ioperms();

int io_owner( int ioport );

int io_grant( struct process *proc, int ioport );
int io_revoke( struct process *proc, int ioport );

int io_grant_priv( struct process *proc );
int io_revoke_priv( struct process *proc );

#endif

