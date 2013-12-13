#ifndef _LIBKERNEL_PORTS_H
#define _LIBKERNEL_PORTS_H

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct port_pair
{
	int port;
	unsigned int flags;
} __attribute__ ((packed));

int smk_create_port( int port, int tid );
int smk_port2tid( int port );
int smk_tid2port( int tid );
int smk_release_port( int port );

int smk_mask_port( int port );
int smk_unmask_port( int port );

int smk_save_port_masks( struct port_pair masks[1024] );
int smk_restore_port_masks( struct port_pair masks[1024] );

int smk_new_port( int tid );

#ifdef __cplusplus
}
#endif

#endif

