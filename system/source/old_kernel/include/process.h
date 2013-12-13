#ifndef _KERNEL_PROCESS_H
#define _KERNEL_PROCESS_H

#include <types.h>
#include <lengths.h>
#include <configure.h>

#include <ports.h>
#include <ipc.h>
#include <htable.h>
#include <rwlock.h>



#define PROCESS_SYSTEM			0x00
#define PROCESS_RUNNING			0x01
#define PROCESS_STOPPED			0x02
#define PROCESS_FINISHING		0x03
#define PROCESS_FINISHED		0x04
#define PROCESS_CRASHING		0x05
#define PROCESS_CRASHED			0x06

extern char *process_states[]; 
extern int process_length;

#include <def_process.h>

// ------------------------------------------

void init_processes();


struct process*		new_process( const char name[OS_NAME_LENGTH] ); 
int 				delete_process( struct process *p );

int 				insert_process( int pid, struct process *p );
struct process*		detach_process( int pid );



// ----------------------------

typedef int (*process_callback)(struct process*);

int foreach_process( process_callback, unsigned int flags );  


int 			find_process( const char name[OS_NAME_LENGTH] );

struct process *checkout_process( int pid,  unsigned int flags );
void 			commit_process( struct process *proc );



int set_process_state( struct process *proc, uint32 state );


#endif

