#ifndef _LIBKERNEL_MESSAGES_H
#define _LIBKERNEL_MESSAGES_H

#include <types.h>


#ifdef __cplusplus
extern "C" {
#endif

int smk_send_message( int source_port, 
		  int dest_pid,
		  int dest_port,
		  int bytes, void *data );

int smk_recv_message( int *source_pid,
	          int *source_port,
		  int *dest_port,
		  int bytes, 
		  void *data );


int smk_peek_message( int *source_pid,
	          int *source_port,
		  int *dest_port,
		  int *bytes);

int smk_drop_message();


#ifdef __cplusplus
}
#endif

#endif

