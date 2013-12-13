#ifndef _KERNEL_RWLOCK_H
#define _KERNEL_RWLOCK_H

#define		READER			0
#define		WRITER			1

struct rwlock
{
	int num_readers;
	int num_writers;
	int write_request;
	volatile unsigned int spinlock;
};


struct rwlock *init_rwlock();
int delete_rwlock( struct rwlock * );

int rwlock_get_write_access( struct rwlock * );
int rwlock_get_read_access( struct rwlock * );
int rwlock_release( struct rwlock * );



#endif



