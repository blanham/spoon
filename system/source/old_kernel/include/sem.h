#ifndef _KERNEL_SEM_H
#define _KERNEL_SEM_H


#include <types.h>
#include <process.h>
#include <threads.h>


struct sem_link
{
	struct sem_link *next;
	int pid;
	int tid;
};


struct semaphore
{
	int sem_id;
	int pid;			
	int capacity;
	int count; 		
	struct sem_link *waiting_list;
};





void init_semaphores();


int create_global_semaphore( int pid, int capacity );
int destroy_global_semaphore( int pid, int sem_id );


int create_local_semaphore( struct process *proc, int capacity );
int destroy_local_semaphore( struct process *proc, int sem_id );

int wait_global_semaphore( struct thread *tr, int sem_id );
int wait_local_semaphore( struct thread *tr, int sem_id );


int signal_global_semaphore( struct thread *tr, int sem_id );
int signal_local_semaphore( struct thread *tr, int sem_id );



#endif

