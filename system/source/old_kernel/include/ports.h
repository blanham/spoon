#ifndef _KERNEL_PORTS_H
#define _KERNEL_PORTS_H

#include <process.h>
#include <threads.h>




int create_port( struct thread *tr, int port, int tid );
int port2tid( struct process *proc, int port );
int tid2port( struct process *proc, int tid );
int release_port( struct thread *tr, int port );

int port_flags( struct process *proc, int port, unsigned int flags );

int get_port_flags( struct thread *tr, int port );
int set_port_flags( struct thread *tr, int port, unsigned int flags );

int bulk_get_ports( struct thread *tr, void *data );
int bulk_set_ports( struct thread *tr, void *data );

int release_thread_ports( struct thread *tr );

#endif

